#include "ForceSolver.h"
#include "../common/Noise.h"

namespace pixelpart {
ForceSolver::ForceSolver() {

}

void ForceSolver::solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t numParticles, float_t t, float_t dt) const {
	for(std::size_t f = 0u; f < forceFields.size(); f++) {
		const ForceField& forceField = forceFields[f];
		if(forceFieldExclusionSets[f][particleType.id] ||
			(t < forceField.lifetimeStart) ||
			(t > forceField.lifetimeStart + forceField.lifetimeDuration && !forceField.repeat)) {
			continue;
		}

		solve(particleType, particles, numParticles, t, dt, forceField);
	}
}

void ForceSolver::solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t numParticles, float_t t, float_t dt, const ForceField& forceField) const {
	float_t life = std::fmod(t - forceField.lifetimeStart, forceField.lifetimeDuration) / forceField.lifetimeDuration;
	vec3_t forceFieldCenter = forceField.position.get(life);
	vec3_t forceFieldSize = forceField.size.get(life) * 0.5;
	float_t forceStrength = forceField.strength.get(life);

	switch(forceField.type) {
		case ForceField::Type::attraction_field: {
			for(uint32_t p = 0u; p < numParticles; p++) {
				vec3_t forceVector = sampleAttractionField(forceField.attractionField,
					forceFieldCenter, forceFieldSize.x,
					particles.globalPosition[p]);

				particles.force[p] += forceVector * forceStrength * particleType.weight.get(particles.life[p]);
			}

			break;
		}

		case ForceField::Type::acceleration_field: {
			vec3_t forceFieldOrientation = glm::radians(forceField.orientation.get(life));
			mat4_t forceFieldOrientationMatrix = glm::yawPitchRoll(-forceFieldOrientation.y, -forceFieldOrientation.z, -forceFieldOrientation.x);
			vec3_t forceDirection = glm::radians(forceField.accelerationField.direction.get(life));
			mat4_t forceDirectionMatrix = glm::yawPitchRoll(forceDirection.y, forceDirection.z, forceDirection.x);

			for(uint32_t p = 0u; p < numParticles; p++) {
				vec3_t forceVector = sampleAccelerationField(forceField.accelerationField,
					forceFieldCenter, forceFieldSize,
					forceFieldOrientationMatrix, forceDirectionMatrix,
					particles.globalPosition[p]);

				particles.force[p] += forceVector * forceStrength * particleType.weight.get(particles.life[p]);
			}

			break;
		}

		case ForceField::Type::vector_field: {
			if(effectResources == nullptr || effectResources->vectorFields.count(forceField.vectorField.resourceId) == 0u) {
				return;
			}

			const VectorFieldResource& vectorFieldResource = effectResources->vectorFields.at(forceField.vectorField.resourceId);
			vec3_t forceFieldOrientation = glm::radians(forceField.orientation.get(life));
			mat4_t forceFieldOrientationMatrix = glm::yawPitchRoll(-forceFieldOrientation.y, -forceFieldOrientation.z, -forceFieldOrientation.x);
			mat4_t forceFieldDirectionMatrix = glm::yawPitchRoll(forceFieldOrientation.y, forceFieldOrientation.z, forceFieldOrientation.x);
			float_t vectorFieldTightness = glm::clamp(forceField.vectorField.tightness.get(life), 0.0, 1.0);

			for(uint32_t p = 0u; p < numParticles; p++) {
				bool inside = false;
				vec3_t forceVector = sampleVectorField(forceField.vectorField, vectorFieldResource,
					forceFieldCenter, forceFieldSize,
					forceFieldOrientationMatrix, forceFieldDirectionMatrix,
					particles.globalPosition[p], inside);

				if(inside) {
					forceVector *= forceStrength * particleType.weight.get(particles.life[p]);
					particles.force[p] += forceVector * (1.0 - vectorFieldTightness);
					particles.velocity[p] *= 1.0 - vectorFieldTightness;
					particles.velocity[p] += forceVector * vectorFieldTightness;
				}
			}

			break;
		}

		case ForceField::Type::noise_field: {
			vec3_t forceFieldOrientation = glm::radians(forceField.orientation.get(life));
			mat4_t forceFieldOrientationMatrix = glm::yawPitchRoll(-forceFieldOrientation.y, -forceFieldOrientation.z, -forceFieldOrientation.x);

			for(uint32_t p = 0u; p < numParticles; p++) {
				vec3_t forceVector = sampleNoiseField(forceField.noiseField,
					forceFieldCenter, forceFieldSize,
					forceFieldOrientationMatrix,
					particles.globalPosition[p],
					life, t);

				particles.force[p] += forceVector * forceStrength * particleType.weight.get(particles.life[p]);
			}

			break;
		}

		case ForceField::Type::drag_field: {
			vec3_t forceFieldOrientation = glm::radians(forceField.orientation.get(life));
			mat4_t forceFieldOrientationMatrix = glm::yawPitchRoll(-forceFieldOrientation.y, -forceFieldOrientation.z, -forceFieldOrientation.x);

			for(uint32_t p = 0u; p < numParticles; p++) {
				vec3_t forceVector = sampleDragField(forceField.dragField,
					forceFieldCenter, forceFieldSize,
					forceFieldOrientationMatrix,
					particles.globalPosition[p], particles.velocity[p], particles.size[p]);

				particles.force[p] += forceVector * forceStrength * particleType.weight.get(particles.life[p]);
			}

			break;
		}

		default: {
			break;
		}
	}
}

void ForceSolver::update(const Effect* effect) {
	if(!effect) {
		effectResources = nullptr;
		is3d = false;
		forceFields.clear();
		forceFieldExclusionSets.clear();
		return;
	}

	effectResources = &effect->resources;
	is3d = effect->is3d;

	forceFields = effect->forceFields.get();

	forceFieldExclusionSets.resize(forceFields.size());
	for(std::size_t f = 0u; f < forceFields.size(); f++) {
		forceFieldExclusionSets[f].reset();
		for(uint32_t particleTypeId : forceFields[f].exclusionList) {
			forceFieldExclusionSets[f].set(particleTypeId);
		}
	}
}

vec3_t ForceSolver::sampleAttractionField(const ForceField::AttractionField& attractionField,
	const vec3_t& position, float_t size,
	const vec3_t& particlePosition) const {
	float_t distanceToCenter = glm::distance(position, particlePosition);
	if(distanceToCenter > size && size > 0.0) {
		return vec3_t(0.0);
	}

	distanceToCenter = std::max(distanceToCenter, 0.01);

	return (position - particlePosition) / (distanceToCenter * distanceToCenter);
}
vec3_t ForceSolver::sampleAccelerationField(const ForceField::AccelerationField& accelerationField,
	const vec3_t& position, const vec3_t& size, const mat4_t& orientationMatrix, const mat4_t& directionMatrix,
	const vec3_t& particlePosition) const {
	vec3_t rotatedParticlePosition = position + vec3_t(orientationMatrix * vec4_t(particlePosition - position, 1.0));
	if(((rotatedParticlePosition.x < position.x - size.x || rotatedParticlePosition.x > position.x + size.x) && size.x > 0.0) ||
		((rotatedParticlePosition.y < position.y - size.y || rotatedParticlePosition.y > position.y + size.y) && size.y > 0.0) ||
		((rotatedParticlePosition.z < position.z - size.z || rotatedParticlePosition.z > position.z + size.z) && size.z > 0.0)) {
		return vec3_t(0.0);
	}

	int32_t gridCellX = (size.x > 0.0)
		? static_cast<int32_t>((rotatedParticlePosition.x - (position.x - size.x)) / (size.x * 2.0) * static_cast<float_t>(accelerationField.gridSize[0]))
		: 0;
	int32_t gridCellY = (size.y > 0.0)
		? static_cast<int32_t>((rotatedParticlePosition.y - (position.y - size.y)) / (size.y * 2.0) * static_cast<float_t>(accelerationField.gridSize[1]))
		: 0;
	int32_t gridCellZ = (size.z > 0.0)
		? static_cast<int32_t>((rotatedParticlePosition.z - (position.z - size.z)) / (size.z * 2.0) * static_cast<float_t>(accelerationField.gridSize[2]))
		: 0;
	uint32_t gridCellIndex = static_cast<uint32_t>(
		gridCellZ * accelerationField.gridSize[1] * accelerationField.gridSize[0] +
		gridCellY * accelerationField.gridSize[0] +
		gridCellX);

	vec3_t gridDirectionOffset = glm::radians(accelerationField.directionVariance.get() * accelerationField.directionGrid[gridCellIndex]);
	float_t gridStrengthOffset = accelerationField.strengthVariance.get() * accelerationField.strengthGrid[gridCellIndex] + 1.0;

	vec3_t result = vec3_t(glm::yawPitchRoll(gridDirectionOffset.y, gridDirectionOffset.z, gridDirectionOffset.x) *
		vec4_t(vec3_t(directionMatrix * worldUpVector4), 0.0));
	result *= gridStrengthOffset;

	return result;
}
vec3_t ForceSolver::sampleVectorField(const ForceField::VectorField& vectorField, const VectorFieldResource& resource,
	const vec3_t& position, const vec3_t& size, const mat4_t& orientationMatrix, const mat4_t& directionMatrix,
	const vec3_t& particlePosition, bool& inside) const {
	vec3_t rotatedParticlePosition = position + vec3_t(orientationMatrix * vec4_t(particlePosition - position, 1.0));
	if(rotatedParticlePosition.x < position.x - size.x || rotatedParticlePosition.x > position.x + size.x ||
		rotatedParticlePosition.y < position.y - size.y || rotatedParticlePosition.y > position.y + size.y ||
		rotatedParticlePosition.z < position.z - size.z || rotatedParticlePosition.z > position.z + size.z) {
		inside = false;
		return vec3_t(0.0);
	}

	vec3_t result = vec3_t(0.0);
	vec3_t samplePosition = (rotatedParticlePosition - position + size) / (size * 2.0);

	if(is3d) {
		switch(vectorField.filter) {
			case ForceField::VectorField::Filter::none: {
				vec3_t normalizedSamplePosition = vec3_t(
					samplePosition.x * static_cast<float_t>(resource.field.getWidth()),
					samplePosition.y * static_cast<float_t>(resource.field.getHeight()),
					samplePosition.z * static_cast<float_t>(resource.field.getDepth()));

				result = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x),
					static_cast<int32_t>(normalizedSamplePosition.y),
					static_cast<int32_t>(normalizedSamplePosition.z),
					vec3_t(0.0));
			}

			case ForceField::VectorField::Filter::linear: {
				vec3_t normalizedSamplePosition = vec3_t(
					samplePosition.x * static_cast<float_t>(resource.field.getWidth()),
					samplePosition.y * static_cast<float_t>(resource.field.getHeight()),
					samplePosition.z * static_cast<float_t>(resource.field.getDepth()));

				float_t fractX = glm::fract(normalizedSamplePosition.x);
				float_t fractY = glm::fract(normalizedSamplePosition.y);
				float_t fractZ = glm::fract(normalizedSamplePosition.y);
				int32_t nextOffsetX = fractX > 0.5 ? +1 : -1;
				int32_t nextOffsetY = fractY > 0.5 ? +1 : -1;
				int32_t nextOffsetZ = fractZ > 0.5 ? +1 : -1;

				vec3_t sample0 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x),
					static_cast<int32_t>(normalizedSamplePosition.y),
					static_cast<int32_t>(normalizedSamplePosition.z),
					vec3_t(0.0));
				vec3_t sample1 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x) + nextOffsetX,
					static_cast<int32_t>(normalizedSamplePosition.y),
					static_cast<int32_t>(normalizedSamplePosition.z),
					vec3_t(0.0));
				vec3_t sample2 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x),
					static_cast<int32_t>(normalizedSamplePosition.y) + nextOffsetY,
					static_cast<int32_t>(normalizedSamplePosition.z),
					vec3_t(0.0));
				vec3_t sample3 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x) + nextOffsetX,
					static_cast<int32_t>(normalizedSamplePosition.y) + nextOffsetY,
					static_cast<int32_t>(normalizedSamplePosition.z),
					vec3_t(0.0));
				vec3_t sample4 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x),
					static_cast<int32_t>(normalizedSamplePosition.y),
					static_cast<int32_t>(normalizedSamplePosition.z) + nextOffsetZ,
					vec3_t(0.0));
				vec3_t sample5 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x) + nextOffsetX,
					static_cast<int32_t>(normalizedSamplePosition.y),
					static_cast<int32_t>(normalizedSamplePosition.z) + nextOffsetZ,
					vec3_t(0.0));
				vec3_t sample6 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x),
					static_cast<int32_t>(normalizedSamplePosition.y) + nextOffsetY,
					static_cast<int32_t>(normalizedSamplePosition.z) + nextOffsetZ,
					vec3_t(0.0));
				vec3_t sample7 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x) + nextOffsetX,
					static_cast<int32_t>(normalizedSamplePosition.y) + nextOffsetY,
					static_cast<int32_t>(normalizedSamplePosition.z) + nextOffsetZ,
					vec3_t(0.0));

				result = glm::lerp(
					glm::lerp(
						glm::lerp(sample0, sample1, glm::abs(fractX - 0.5)),
						glm::lerp(sample2, sample3, glm::abs(fractX - 0.5)),
						glm::abs(fractY - 0.5)),
					glm::lerp(
						glm::lerp(sample4, sample5, glm::abs(fractX - 0.5)),
						glm::lerp(sample6, sample7, glm::abs(fractX - 0.5)),
						glm::abs(fractY - 0.5)),
					glm::abs(fractZ - 0.5));
			}

			default: {
				break;
			}
		}
	}
	else {
		switch(vectorField.filter) {
			case ForceField::VectorField::Filter::none: {
				vec3_t normalizedSamplePosition = vec3_t(
					samplePosition.x * static_cast<float_t>(resource.field.getWidth()),
					samplePosition.y * static_cast<float_t>(resource.field.getHeight()),
					0.0);

				result = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x),
					static_cast<int32_t>(normalizedSamplePosition.y),
					static_cast<int32_t>(normalizedSamplePosition.z),
					vec3_t(0.0));
			}

			case ForceField::VectorField::Filter::linear: {
				vec3_t normalizedSamplePosition = vec3_t(
					samplePosition.x * static_cast<float_t>(resource.field.getWidth()),
					samplePosition.y * static_cast<float_t>(resource.field.getHeight()),
					0.0);

				float_t fractX = glm::fract(normalizedSamplePosition.x);
				float_t fractY = glm::fract(normalizedSamplePosition.y);
				int32_t nextOffsetX = fractX > 0.5 ? +1 : -1;
				int32_t nextOffsetY = fractY > 0.5 ? +1 : -1;

				vec3_t sample0 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x),
					static_cast<int32_t>(normalizedSamplePosition.y),
					static_cast<int32_t>(normalizedSamplePosition.z),
					vec3_t(0.0));
				vec3_t sample1 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x) + nextOffsetX,
					static_cast<int32_t>(normalizedSamplePosition.y),
					static_cast<int32_t>(normalizedSamplePosition.z),
					vec3_t(0.0));
				vec3_t sample2 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x),
					static_cast<int32_t>(normalizedSamplePosition.y) + nextOffsetY,
					static_cast<int32_t>(normalizedSamplePosition.z),
					vec3_t(0.0));
				vec3_t sample3 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x) + nextOffsetX,
					static_cast<int32_t>(normalizedSamplePosition.y) + nextOffsetY,
					static_cast<int32_t>(normalizedSamplePosition.z),
					vec3_t(0.0));

				result = glm::lerp(
					glm::lerp(sample0, sample1, glm::abs(fractX - 0.5)),
					glm::lerp(sample2, sample3, glm::abs(fractX - 0.5)),
					glm::abs(fractY - 0.5));
			}

			default: {
				break;
			}
		}
	}

	inside = true;

	return vec3_t(directionMatrix * vec4_t(result, 0.0));
}
vec3_t ForceSolver::sampleNoiseField(const ForceField::NoiseField& noiseField,
	const vec3_t& position, const vec3_t& size, const mat4_t& orientationMatrix,
	const vec3_t& particlePosition, float_t life, float_t t) const {
	vec3_t rotatedParticlePosition = position + vec3_t(orientationMatrix * vec4_t(particlePosition - position, 1.0));
	if(((rotatedParticlePosition.x < position.x - size.x || rotatedParticlePosition.x > position.x + size.x) && size.x > 0.0) ||
		((rotatedParticlePosition.y < position.y - size.y || rotatedParticlePosition.y > position.y + size.y) && size.y > 0.0) ||
		((rotatedParticlePosition.z < position.z - size.z || rotatedParticlePosition.z > position.z + size.z) && size.z > 0.0)) {
		return vec3_t(0.0);
	}

	vec3_t samplePosition = rotatedParticlePosition - position;
	uint32_t octaves = static_cast<uint32_t>(std::max(noiseField.octaves.get(), static_cast<int64_t>(0)));
	float_t frequency = noiseField.frequency.get(life);
	float_t persistence = noiseField.persistence.get(life);
	float_t lacunarity = noiseField.lacunarity.get(life);

	if(noiseField.animated) {
		float_t animationTime = noiseField.animationTimeBase.get() + noiseField.animationTimeScale.get() * t;

		return is3d
			? computeAnimatedCurlNoise3d(samplePosition, animationTime, octaves, frequency, persistence, lacunarity)
			: computeAnimatedCurlNoise2d(samplePosition, animationTime, octaves, frequency, persistence, lacunarity);
	}
	else {
		return is3d
			? computeStaticCurlNoise3d(samplePosition, octaves, frequency, persistence, lacunarity)
			: computeStaticCurlNoise2d(samplePosition, octaves, frequency, persistence, lacunarity);
	}
}
vec3_t ForceSolver::sampleDragField(const ForceField::DragField& dragField,
	const vec3_t& position, const vec3_t& size, const mat4_t& orientationMatrix,
	const vec3_t& particlePosition, const vec3_t& particleVelocity, const vec3_t& particleSize) const {
	vec3_t rotatedParticlePosition = position + vec3_t(orientationMatrix * vec4_t(particlePosition - position, 1.0));
	if(((rotatedParticlePosition.x < position.x - size.x || rotatedParticlePosition.x > position.x + size.x) && size.x > 0.0) ||
		((rotatedParticlePosition.y < position.y - size.y || rotatedParticlePosition.y > position.y + size.y) && size.y > 0.0) ||
		((rotatedParticlePosition.z < position.z - size.z || rotatedParticlePosition.z > position.z + size.z) && size.z > 0.0)) {
		return vec3_t(0.0);
	}

	float_t particleSpeed = std::max(glm::length(particleVelocity), 0.001);
	float_t particleArea = std::max(particleSize.x, std::max(particleSize.y, particleSize.z));

	return -particleVelocity / particleSpeed *
		(1.0 + (particleSpeed * particleSpeed - 1.0) * dragField.velocityInfluence.get()) *
		(1.0 + (particleArea - 1.0) * dragField.sizeInfluence.get());
}

vec3_t ForceSolver::computeStaticCurlNoise2d(const vec2_t& samplePosition, uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const {
	const float_t epsilon = 1.0e-4;

	float_t x1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + epsilon,
		samplePosition.y);
	float_t x2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x - epsilon,
		samplePosition.y);

	float_t y1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y + epsilon);
	float_t y2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y - epsilon);

	return vec3_t(
		(x1 - x2) / epsilon * 0.5,
		(y1 - y2) / epsilon * 0.5,
		0.0);
}
vec3_t ForceSolver::computeStaticCurlNoise3d(const vec3_t& samplePosition, uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const {
	const float_t epsilon = 1.0e-4;
	const float_t offset = 1000.0;

	float_t x1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + epsilon,
		samplePosition.y,
		samplePosition.z);
	float_t x2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x - epsilon,
		samplePosition.y,
		samplePosition.z);

	float_t y1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y + epsilon,
		samplePosition.z);
	float_t y2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y - epsilon,
		samplePosition.z);

	float_t z1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y,
		samplePosition.z + epsilon);
	float_t z2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y,
		samplePosition.z - epsilon);

	vec3_t noiseGradient1 = vec3_t(
		(x1 - x2) / epsilon * 0.5,
		(y1 - y2) / epsilon * 0.5,
		(z1 - z2) / epsilon * 0.5);

	x1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset + epsilon,
		samplePosition.y + offset,
		samplePosition.z + offset);
	x2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset - epsilon,
		samplePosition.y + offset,
		samplePosition.z + offset);

	y1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset,
		samplePosition.y + offset + epsilon,
		samplePosition.z + offset);
	y2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset,
		samplePosition.y + offset - epsilon,
		samplePosition.z + offset);

	z1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset,
		samplePosition.y + offset,
		samplePosition.z + offset + epsilon);
	z2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset,
		samplePosition.y + offset,
		samplePosition.z + offset - epsilon);

	vec3_t noiseGradient2 = vec3_t(
		(x1 - x2) / epsilon * 0.5,
		(y1 - y2) / epsilon * 0.5,
		(z1 - z2) / epsilon * 0.5);

	return glm::normalize(glm::cross(
		glm::normalize(noiseGradient1),
		glm::normalize(noiseGradient2)));
}
vec3_t ForceSolver::computeAnimatedCurlNoise2d(const vec2_t& samplePosition, float_t animationTime, uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const {
	const float_t epsilon = 1.0e-4;

	float_t x1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + epsilon,
		samplePosition.y,
		animationTime);
	float_t x2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x - epsilon,
		samplePosition.y,
		animationTime);

	float_t y1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y + epsilon,
		animationTime);
	float_t y2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y - epsilon,
		animationTime);

	return vec3_t(
		(x1 - x2) / epsilon * 0.5,
		(y1 - y2) / epsilon * 0.5,
		0.0);
}
vec3_t ForceSolver::computeAnimatedCurlNoise3d(const vec3_t& samplePosition, float_t animationTime, uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const {
	const float_t epsilon = 1.0e-4;
	const float_t offset = 1000.0;

	float_t x1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + epsilon,
		samplePosition.y,
		samplePosition.z,
		animationTime);
	float_t x2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x - epsilon,
		samplePosition.y,
		samplePosition.z,
		animationTime);

	float_t y1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y + epsilon,
		samplePosition.z,
		animationTime);
	float_t y2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y - epsilon,
		samplePosition.z,
		animationTime);

	float_t z1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y,
		samplePosition.z + epsilon,
		animationTime);
	float_t z2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y,
		samplePosition.z - epsilon,
		animationTime);

	vec3_t noiseGradient1 = vec3_t(
		(x1 - x2) / epsilon * 0.5,
		(y1 - y2) / epsilon * 0.5,
		(z1 - z2) / epsilon * 0.5);

	x1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset + epsilon,
		samplePosition.y + offset,
		samplePosition.z + offset,
		animationTime);
	x2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset - epsilon,
		samplePosition.y + offset,
		samplePosition.z + offset,
		animationTime);

	y1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset,
		samplePosition.y + offset + epsilon,
		samplePosition.z + offset,
		animationTime);
	y2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset,
		samplePosition.y + offset - epsilon,
		samplePosition.z + offset,
		animationTime);

	z1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset,
		samplePosition.y + offset,
		samplePosition.z + offset + epsilon,
		animationTime);
	z2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset,
		samplePosition.y + offset,
		samplePosition.z + offset - epsilon,
		animationTime);

	vec3_t noiseGradient2 = vec3_t(
		(x1 - x2) / epsilon * 0.5,
		(y1 - y2) / epsilon * 0.5,
		(z1 - z2) / epsilon * 0.5);

	return glm::normalize(glm::cross(
		glm::normalize(noiseGradient1),
		glm::normalize(noiseGradient2)));
}
}