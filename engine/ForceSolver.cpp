#include "ForceSolver.h"
#include "../common/Noise.h"

namespace pixelpart {
ForceSolver::ForceSolver() {

}

void ForceSolver::solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t numParticles, floatd t, floatd dt) const {
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

void ForceSolver::solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t numParticles, floatd t, floatd dt, const ForceField& forceField) const {
	floatd life = std::fmod(t - forceField.lifetimeStart, forceField.lifetimeDuration) / forceField.lifetimeDuration;
	vec3d forceFieldCenter = forceField.position.get(life);
	vec3d forceFieldSize = forceField.size.get(life) * 0.5;
	floatd forceStrength = forceField.strength.get(life);

	switch(forceField.type) {
		case ForceField::Type::attraction_field: {
			for(uint32_t p = 0u; p < numParticles; p++) {
				vec3d forceVector = sampleAttractionField(forceField.attractionField,
					forceFieldCenter, forceFieldSize.x,
					particles.globalPosition[p]);

				particles.force[p] += forceVector * forceStrength * particleType.weight.get(particles.life[p]);
			}

			break;
		}

		case ForceField::Type::acceleration_field: {
			vec3d forceFieldOrientation = glm::radians(forceField.orientation.get(life));
			mat4d forceFieldOrientationMatrix = glm::yawPitchRoll(-forceFieldOrientation.y, -forceFieldOrientation.z, -forceFieldOrientation.x);
			vec3d forceDirection = glm::radians(forceField.accelerationField.direction.get(life));
			mat4d forceDirectionMatrix = glm::yawPitchRoll(forceDirection.y, forceDirection.z, forceDirection.x);

			for(uint32_t p = 0u; p < numParticles; p++) {
				vec3d forceVector = sampleAccelerationField(forceField.accelerationField,
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
			vec3d forceFieldOrientation = glm::radians(forceField.orientation.get(life));
			mat4d forceFieldOrientationMatrix = glm::yawPitchRoll(-forceFieldOrientation.y, -forceFieldOrientation.z, -forceFieldOrientation.x);
			mat4d forceFieldDirectionMatrix = glm::yawPitchRoll(forceFieldOrientation.y, forceFieldOrientation.z, forceFieldOrientation.x);
			floatd vectorFieldTightness = glm::clamp(forceField.vectorField.tightness.get(life), 0.0, 1.0);

			for(uint32_t p = 0u; p < numParticles; p++) {
				bool inside = false;
				vec3d forceVector = sampleVectorField(forceField.vectorField, vectorFieldResource,
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
			vec3d forceFieldOrientation = glm::radians(forceField.orientation.get(life));
			mat4d forceFieldOrientationMatrix = glm::yawPitchRoll(-forceFieldOrientation.y, -forceFieldOrientation.z, -forceFieldOrientation.x);

			for(uint32_t p = 0u; p < numParticles; p++) {
				vec3d forceVector = sampleNoiseField(forceField.noiseField,
					forceFieldCenter, forceFieldSize,
					forceFieldOrientationMatrix,
					particles.globalPosition[p],
					life, t);

				particles.force[p] += forceVector * forceStrength * particleType.weight.get(particles.life[p]);
			}

			break;
		}

		case ForceField::Type::drag_field: {
			vec3d forceFieldOrientation = glm::radians(forceField.orientation.get(life));
			mat4d forceFieldOrientationMatrix = glm::yawPitchRoll(-forceFieldOrientation.y, -forceFieldOrientation.z, -forceFieldOrientation.x);

			for(uint32_t p = 0u; p < numParticles; p++) {
				vec3d forceVector = sampleDragField(forceField.dragField,
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

vec3d ForceSolver::sampleAttractionField(const ForceField::AttractionField& attractionField,
	const vec3d& position, floatd size,
	const vec3d& particlePosition) const {
	floatd distanceToCenter = glm::distance(position, particlePosition);
	if(distanceToCenter > size && size > 0.0) {
		return vec3d(0.0);
	}

	distanceToCenter = std::max(distanceToCenter, 0.01);

	return (position - particlePosition) / (distanceToCenter * distanceToCenter);
}
vec3d ForceSolver::sampleAccelerationField(const ForceField::AccelerationField& accelerationField,
	const vec3d& position, const vec3d& size, const mat4d& orientationMatrix, const mat4d& directionMatrix,
	const vec3d& particlePosition) const {
	vec3d rotatedParticlePosition = position + vec3d(orientationMatrix * vec4d(particlePosition - position, 1.0));
	if(((rotatedParticlePosition.x < position.x - size.x || rotatedParticlePosition.x > position.x + size.x) && size.x > 0.0) ||
		((rotatedParticlePosition.y < position.y - size.y || rotatedParticlePosition.y > position.y + size.y) && size.y > 0.0) ||
		((rotatedParticlePosition.z < position.z - size.z || rotatedParticlePosition.z > position.z + size.z) && size.z > 0.0)) {
		return vec3d(0.0);
	}

	int32_t gridCellX = (size.x > 0.0)
		? static_cast<int32_t>((rotatedParticlePosition.x - (position.x - size.x)) / (size.x * 2.0) * static_cast<floatd>(accelerationField.gridSize[0]))
		: 0;
	int32_t gridCellY = (size.y > 0.0)
		? static_cast<int32_t>((rotatedParticlePosition.y - (position.y - size.y)) / (size.y * 2.0) * static_cast<floatd>(accelerationField.gridSize[1]))
		: 0;
	int32_t gridCellZ = (size.z > 0.0)
		? static_cast<int32_t>((rotatedParticlePosition.z - (position.z - size.z)) / (size.z * 2.0) * static_cast<floatd>(accelerationField.gridSize[2]))
		: 0;
	uint32_t gridCellIndex = static_cast<uint32_t>(
		gridCellZ * accelerationField.gridSize[1] * accelerationField.gridSize[0] +
		gridCellY * accelerationField.gridSize[0] +
		gridCellX);

	vec3d gridDirectionOffset = glm::radians(accelerationField.directionVariance.get() * accelerationField.directionGrid[gridCellIndex]);
	floatd gridStrengthOffset = accelerationField.strengthVariance.get() * accelerationField.strengthGrid[gridCellIndex] + 1.0;

	vec3d result = vec3d(glm::yawPitchRoll(gridDirectionOffset.y, gridDirectionOffset.z, gridDirectionOffset.x) *
		vec4d(vec3d(directionMatrix * worldUpVector4), 0.0));
	result *= gridStrengthOffset;

	return result;
}
vec3d ForceSolver::sampleVectorField(const ForceField::VectorField& vectorField, const VectorFieldResource& resource,
	const vec3d& position, const vec3d& size, const mat4d& orientationMatrix, const mat4d& directionMatrix,
	const vec3d& particlePosition, bool& inside) const {
	vec3d rotatedParticlePosition = position + vec3d(orientationMatrix * vec4d(particlePosition - position, 1.0));
	if(rotatedParticlePosition.x < position.x - size.x || rotatedParticlePosition.x > position.x + size.x ||
		rotatedParticlePosition.y < position.y - size.y || rotatedParticlePosition.y > position.y + size.y ||
		rotatedParticlePosition.z < position.z - size.z || rotatedParticlePosition.z > position.z + size.z) {
		inside = false;
		return vec3d(0.0);
	}

	vec3d result = vec3d(0.0);
	vec3d samplePosition = (rotatedParticlePosition - position + size) / (size * 2.0);

	if(is3d) {
		switch(vectorField.filter) {
			case ForceField::VectorField::Filter::none: {
				vec3d normalizedSamplePosition = vec3d(
					samplePosition.x * static_cast<floatd>(resource.field.getWidth()),
					samplePosition.y * static_cast<floatd>(resource.field.getHeight()),
					samplePosition.z * static_cast<floatd>(resource.field.getDepth()));

				result = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x),
					static_cast<int32_t>(normalizedSamplePosition.y),
					static_cast<int32_t>(normalizedSamplePosition.z),
					vec3d(0.0));
			}

			case ForceField::VectorField::Filter::linear: {
				vec3d normalizedSamplePosition = vec3d(
					samplePosition.x * static_cast<floatd>(resource.field.getWidth()),
					samplePosition.y * static_cast<floatd>(resource.field.getHeight()),
					samplePosition.z * static_cast<floatd>(resource.field.getDepth()));

				floatd fractX = glm::fract(normalizedSamplePosition.x);
				floatd fractY = glm::fract(normalizedSamplePosition.y);
				floatd fractZ = glm::fract(normalizedSamplePosition.y);
				int32_t nextOffsetX = fractX > 0.5 ? +1 : -1;
				int32_t nextOffsetY = fractY > 0.5 ? +1 : -1;
				int32_t nextOffsetZ = fractZ > 0.5 ? +1 : -1;

				vec3d sample0 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x),
					static_cast<int32_t>(normalizedSamplePosition.y),
					static_cast<int32_t>(normalizedSamplePosition.z),
					vec3d(0.0));
				vec3d sample1 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x) + nextOffsetX,
					static_cast<int32_t>(normalizedSamplePosition.y),
					static_cast<int32_t>(normalizedSamplePosition.z),
					vec3d(0.0));
				vec3d sample2 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x),
					static_cast<int32_t>(normalizedSamplePosition.y) + nextOffsetY,
					static_cast<int32_t>(normalizedSamplePosition.z),
					vec3d(0.0));
				vec3d sample3 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x) + nextOffsetX,
					static_cast<int32_t>(normalizedSamplePosition.y) + nextOffsetY,
					static_cast<int32_t>(normalizedSamplePosition.z),
					vec3d(0.0));
				vec3d sample4 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x),
					static_cast<int32_t>(normalizedSamplePosition.y),
					static_cast<int32_t>(normalizedSamplePosition.z) + nextOffsetZ,
					vec3d(0.0));
				vec3d sample5 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x) + nextOffsetX,
					static_cast<int32_t>(normalizedSamplePosition.y),
					static_cast<int32_t>(normalizedSamplePosition.z) + nextOffsetZ,
					vec3d(0.0));
				vec3d sample6 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x),
					static_cast<int32_t>(normalizedSamplePosition.y) + nextOffsetY,
					static_cast<int32_t>(normalizedSamplePosition.z) + nextOffsetZ,
					vec3d(0.0));
				vec3d sample7 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x) + nextOffsetX,
					static_cast<int32_t>(normalizedSamplePosition.y) + nextOffsetY,
					static_cast<int32_t>(normalizedSamplePosition.z) + nextOffsetZ,
					vec3d(0.0));

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
				vec3d normalizedSamplePosition = vec3d(
					samplePosition.x * static_cast<floatd>(resource.field.getWidth()),
					samplePosition.y * static_cast<floatd>(resource.field.getHeight()),
					0.0);

				result = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x),
					static_cast<int32_t>(normalizedSamplePosition.y),
					static_cast<int32_t>(normalizedSamplePosition.z),
					vec3d(0.0));
			}

			case ForceField::VectorField::Filter::linear: {
				vec3d normalizedSamplePosition = vec3d(
					samplePosition.x * static_cast<floatd>(resource.field.getWidth()),
					samplePosition.y * static_cast<floatd>(resource.field.getHeight()),
					0.0);

				floatd fractX = glm::fract(normalizedSamplePosition.x);
				floatd fractY = glm::fract(normalizedSamplePosition.y);
				int32_t nextOffsetX = fractX > 0.5 ? +1 : -1;
				int32_t nextOffsetY = fractY > 0.5 ? +1 : -1;

				vec3d sample0 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x),
					static_cast<int32_t>(normalizedSamplePosition.y),
					static_cast<int32_t>(normalizedSamplePosition.z),
					vec3d(0.0));
				vec3d sample1 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x) + nextOffsetX,
					static_cast<int32_t>(normalizedSamplePosition.y),
					static_cast<int32_t>(normalizedSamplePosition.z),
					vec3d(0.0));
				vec3d sample2 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x),
					static_cast<int32_t>(normalizedSamplePosition.y) + nextOffsetY,
					static_cast<int32_t>(normalizedSamplePosition.z),
					vec3d(0.0));
				vec3d sample3 = resource.field.getOrDefault(
					static_cast<int32_t>(normalizedSamplePosition.x) + nextOffsetX,
					static_cast<int32_t>(normalizedSamplePosition.y) + nextOffsetY,
					static_cast<int32_t>(normalizedSamplePosition.z),
					vec3d(0.0));

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

	return vec3d(directionMatrix * vec4d(result, 0.0));
}
vec3d ForceSolver::sampleNoiseField(const ForceField::NoiseField& noiseField,
	const vec3d& position, const vec3d& size, const mat4d& orientationMatrix,
	const vec3d& particlePosition, floatd life, floatd t) const {
	vec3d rotatedParticlePosition = position + vec3d(orientationMatrix * vec4d(particlePosition - position, 1.0));
	if(((rotatedParticlePosition.x < position.x - size.x || rotatedParticlePosition.x > position.x + size.x) && size.x > 0.0) ||
		((rotatedParticlePosition.y < position.y - size.y || rotatedParticlePosition.y > position.y + size.y) && size.y > 0.0) ||
		((rotatedParticlePosition.z < position.z - size.z || rotatedParticlePosition.z > position.z + size.z) && size.z > 0.0)) {
		return vec3d(0.0);
	}

	vec3d samplePosition = rotatedParticlePosition - position;
	uint32_t octaves = static_cast<uint32_t>(std::max(noiseField.octaves.get(), 0l));
	floatd frequency = noiseField.frequency.get(life);
	floatd persistence = noiseField.persistence.get(life);
	floatd lacunarity = noiseField.lacunarity.get(life);

	if(noiseField.animated) {
		floatd animationTime = noiseField.animationTimeBase.get() + noiseField.animationTimeScale.get() * t;

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
vec3d ForceSolver::sampleDragField(const ForceField::DragField& dragField,
	const vec3d& position, const vec3d& size, const mat4d& orientationMatrix,
	const vec3d& particlePosition, const vec3d& particleVelocity, const vec3d& particleSize) const {
	vec3d rotatedParticlePosition = position + vec3d(orientationMatrix * vec4d(particlePosition - position, 1.0));
	if(((rotatedParticlePosition.x < position.x - size.x || rotatedParticlePosition.x > position.x + size.x) && size.x > 0.0) ||
		((rotatedParticlePosition.y < position.y - size.y || rotatedParticlePosition.y > position.y + size.y) && size.y > 0.0) ||
		((rotatedParticlePosition.z < position.z - size.z || rotatedParticlePosition.z > position.z + size.z) && size.z > 0.0)) {
		return vec3d(0.0);
	}

	floatd particleSpeed = std::max(glm::length(particleVelocity), 0.001);
	floatd particleArea = std::max(particleSize.x, std::max(particleSize.y, particleSize.z));

	return -particleVelocity / particleSpeed *
		(1.0 + (particleSpeed * particleSpeed - 1.0) * dragField.velocityInfluence.get()) *
		(1.0 + (particleArea - 1.0) * dragField.sizeInfluence.get());
}

vec3d ForceSolver::computeStaticCurlNoise2d(const vec2d& samplePosition, uint32_t octaves, floatd frequency, floatd persistence, floatd lacunarity) const {
	const floatd epsilon = 1.0e-4;

	floatd x1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + epsilon,
		samplePosition.y);
	floatd x2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x - epsilon,
		samplePosition.y);

	floatd y1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y + epsilon);
	floatd y2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y - epsilon);

	return vec3d(
		(x1 - x2) / epsilon * 0.5,
		(y1 - y2) / epsilon * 0.5,
		0.0);
}
vec3d ForceSolver::computeStaticCurlNoise3d(const vec3d& samplePosition, uint32_t octaves, floatd frequency, floatd persistence, floatd lacunarity) const {
	const floatd epsilon = 1.0e-4;
	const floatd offset = 1000.0;

	floatd x1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + epsilon,
		samplePosition.y,
		samplePosition.z);
	floatd x2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x - epsilon,
		samplePosition.y,
		samplePosition.z);

	floatd y1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y + epsilon,
		samplePosition.z);
	floatd y2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y - epsilon,
		samplePosition.z);

	floatd z1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y,
		samplePosition.z + epsilon);
	floatd z2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y,
		samplePosition.z - epsilon);

	vec3d noiseGradient1 = vec3d(
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

	vec3d noiseGradient2 = vec3d(
		(x1 - x2) / epsilon * 0.5,
		(y1 - y2) / epsilon * 0.5,
		(z1 - z2) / epsilon * 0.5);

	return glm::normalize(glm::cross(
		glm::normalize(noiseGradient1),
		glm::normalize(noiseGradient2)));
}
vec3d ForceSolver::computeAnimatedCurlNoise2d(const vec2d& samplePosition, floatd animationTime, uint32_t octaves, floatd frequency, floatd persistence, floatd lacunarity) const {
	const floatd epsilon = 1.0e-4;

	floatd x1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + epsilon,
		samplePosition.y,
		animationTime);
	floatd x2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x - epsilon,
		samplePosition.y,
		animationTime);

	floatd y1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y + epsilon,
		animationTime);
	floatd y2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y - epsilon,
		animationTime);

	return vec3d(
		(x1 - x2) / epsilon * 0.5,
		(y1 - y2) / epsilon * 0.5,
		0.0);
}
vec3d ForceSolver::computeAnimatedCurlNoise3d(const vec3d& samplePosition, floatd animationTime, uint32_t octaves, floatd frequency, floatd persistence, floatd lacunarity) const {
	const floatd epsilon = 1.0e-4;
	const floatd offset = 1000.0;

	floatd x1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + epsilon,
		samplePosition.y,
		samplePosition.z,
		animationTime);
	floatd x2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x - epsilon,
		samplePosition.y,
		samplePosition.z,
		animationTime);

	floatd y1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y + epsilon,
		samplePosition.z,
		animationTime);
	floatd y2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y - epsilon,
		samplePosition.z,
		animationTime);

	floatd z1 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y,
		samplePosition.z + epsilon,
		animationTime);
	floatd z2 = noise::fBmSimplexNoise(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y,
		samplePosition.z - epsilon,
		animationTime);

	vec3d noiseGradient1 = vec3d(
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

	vec3d noiseGradient2 = vec3d(
		(x1 - x2) / epsilon * 0.5,
		(y1 - y2) / epsilon * 0.5,
		(z1 - z2) / epsilon * 0.5);

	return glm::normalize(glm::cross(
		glm::normalize(noiseGradient1),
		glm::normalize(noiseGradient2)));
}
}