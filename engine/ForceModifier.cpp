#include "ForceModifier.h"
#include "Noise.h"
#include "../common/Transform.h"
#include "../effect/SceneGraph.h"
#include "../effect/ParticleType.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "../glm/gtx/euler_angles.hpp"
#include <cmath>
#include <iterator>
#include <algorithm>

namespace pixelpart {
void ForceModifier::run(const Effect* effect, RuntimeContext runtimeContext, ParticleRuntimeId runtimeId,
	ParticleCollection::WritePtr particles, std::uint32_t particleCount) const {
	const ParticleType& particleType = effect->particleTypes().at(runtimeId.typeId);

	for(const AttractionField& field : attractionFields) {
		if(field.exclusionSet().count(particleType.id()) != 0) {
			continue;
		}

		applyForce(particles, particleCount, runtimeContext, particleType, field, effect->sceneGraph());
	}

	for(const AccelerationField& field : accelerationFields) {
		if(field.exclusionSet().count(particleType.id()) != 0) {
			continue;
		}

		applyForce(particles, particleCount, runtimeContext, particleType, field, effect->sceneGraph());
	}

	for(const VectorField& field : vectorFields) {
		if(field.exclusionSet().count(particleType.id()) != 0) {
			continue;
		}

		applyForce(particles, particleCount, runtimeContext, particleType, field, effect->sceneGraph());
	}

	for(const NoiseField& field : noiseFields) {
		if(field.exclusionSet().count(particleType.id()) != 0) {
			continue;
		}

		applyForce(particles, particleCount, runtimeContext, particleType, field, effect->sceneGraph());
	}

	for(const DragField& field : dragFields) {
		if(field.exclusionSet().count(particleType.id()) != 0) {
			continue;
		}

		applyForce(particles, particleCount, runtimeContext, particleType, field, effect->sceneGraph());
	}
}

void ForceModifier::prepare(const Effect& effect, const RuntimeContext& runtimeContext) {
	effectResources = &effect.resources();
	is3d = effect.is3d();

	attractionFields.clear();
	accelerationFields.clear();
	vectorFields.clear();
	noiseFields.clear();
	dragFields.clear();

	for(const ForceField* forceField : effect.sceneGraph().nodesWithType<ForceField>()) {
		if(!forceField->active(runtimeContext)) {
			continue;
		}

		const pixelpart::AttractionField* attractionField = dynamic_cast<const pixelpart::AttractionField*>(forceField);
		const pixelpart::AccelerationField* accelerationField = dynamic_cast<const pixelpart::AccelerationField*>(forceField);
		const pixelpart::VectorField* vectorField = dynamic_cast<const pixelpart::VectorField*>(forceField);
		const pixelpart::NoiseField* noiseField = dynamic_cast<const pixelpart::NoiseField*>(forceField);
		const pixelpart::DragField* dragField = dynamic_cast<const pixelpart::DragField*>(forceField);

		if(attractionField) {
			attractionFields.emplace_back(*attractionField);
		}
		else if(accelerationField) {
			accelerationFields.emplace_back(*accelerationField);
		}
		else if(vectorField) {
			vectorFields.emplace_back(*vectorField);
		}
		else if(noiseField) {
			noiseFields.emplace_back(*noiseField);
		}
		else if(dragField) {
			dragFields.emplace_back(*dragField);
		}
	}
}

void ForceModifier::applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const RuntimeContext& runtimeContext,
	const ParticleType& particleType, const AttractionField& attractionField, const SceneGraph& sceneGraph) const {
	float_t alpha = attractionField.life(runtimeContext);
	Transform transform = sceneGraph.globalTransform(attractionField.id(), runtimeContext);
	float3_t center = transform.position();
	float3_t size = transform.scale() * 0.5;
	float_t strength = attractionField.strength().at(alpha);

	for(std::uint32_t p = 0; p < particleCount; p++) {
		float3_t forceVector = sampleAttractionField(attractionField,
			center, size.x,
			particles.globalPosition[p]);

		particles.force[p] += forceVector * strength * particleType.weight().at(particles.life[p]);
	}
}
void ForceModifier::applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const RuntimeContext& runtimeContext,
	const ParticleType& particleType, const AccelerationField& accelerationField, const SceneGraph& sceneGraph) const {
	float_t alpha = accelerationField.life(runtimeContext);
	Transform transform = sceneGraph.globalTransform(accelerationField.id(), runtimeContext);
	float3_t center = transform.position();
	float3_t size = transform.scale() * 0.5;
	float3_t rotation = glm::radians(transform.rotation());
	mat4_t rotationMatrix = glm::yawPitchRoll(-rotation.y, -rotation.z, -rotation.x);
	float3_t direction = glm::radians(accelerationField.accelerationDirection().at(alpha));
	mat4_t directionMatrix = glm::yawPitchRoll(direction.y, direction.z, direction.x);
	float_t strength = accelerationField.strength().at(alpha);

	for(std::uint32_t p = 0; p < particleCount; p++) {
		float3_t forceVector = sampleAccelerationField(accelerationField,
			center, size,
			rotationMatrix, directionMatrix,
			particles.globalPosition[p]);

		particles.force[p] += forceVector * strength * particleType.weight().at(particles.life[p]);
	}
}
void ForceModifier::applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const RuntimeContext& runtimeContext,
	const ParticleType& particleType, const VectorField& vectorField, const SceneGraph& sceneGraph) const {
	if(effectResources == nullptr || effectResources->vectorFields().count(vectorField.vectorFieldResourceId()) == 0) {
		return;
	}

	float_t alpha = vectorField.life(runtimeContext);
	Transform transform = sceneGraph.globalTransform(vectorField.id(), runtimeContext);
	float3_t center = transform.position();
	float3_t size = transform.scale() * 0.5;
	float3_t rotation = glm::radians(transform.rotation());
	mat4_t rotationMatrix = glm::yawPitchRoll(-rotation.y, -rotation.z, -rotation.x);
	float_t strength = vectorField.strength().at(alpha);

	const VectorFieldResource& vectorFieldResource = effectResources->vectorFields().at(vectorField.vectorFieldResourceId());
	mat4_t directionMatrix = glm::yawPitchRoll(rotation.y, rotation.z, rotation.x);
	float_t tightness = glm::clamp(vectorField.tightness().at(alpha), 0.0, 1.0);

	for(std::uint32_t p = 0; p < particleCount; p++) {
		bool inside = false;
		float3_t forceVector = sampleVectorField(vectorField, vectorFieldResource,
			center, size,
			rotationMatrix, directionMatrix,
			particles.globalPosition[p], inside);

		if(inside) {
			forceVector *= strength * particleType.weight().at(particles.life[p]);
			particles.force[p] += forceVector * (1.0 - tightness);
			particles.velocity[p] *= 1.0 - tightness;
			particles.velocity[p] += forceVector * tightness;
		}
	}
}
void ForceModifier::applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const RuntimeContext& runtimeContext,
	const ParticleType& particleType, const NoiseField& noiseField, const SceneGraph& sceneGraph) const {
	float_t t = runtimeContext.time();
	float_t alpha = noiseField.life(runtimeContext);
	Transform transform = sceneGraph.globalTransform(noiseField.id(), runtimeContext);
	float3_t center = transform.position();
	float3_t size = transform.scale() * 0.5;
	float3_t rotation = glm::radians(transform.rotation());
	mat4_t rotationMatrix = glm::yawPitchRoll(-rotation.y, -rotation.z, -rotation.x);
	float_t strength = noiseField.strength().at(alpha);

	for(std::uint32_t p = 0; p < particleCount; p++) {
		float3_t forceVector = sampleNoiseField(noiseField,
			center, size,
			rotationMatrix,
			particles.globalPosition[p],
			alpha, t);

		particles.force[p] += forceVector * strength * particleType.weight().at(particles.life[p]);
	}
}
void ForceModifier::applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const RuntimeContext& runtimeContext,
	const ParticleType& particleType, const DragField& dragField, const SceneGraph& sceneGraph) const {
	float_t alpha = dragField.life(runtimeContext);
	Transform transform = sceneGraph.globalTransform(dragField.id(), runtimeContext);
	float3_t center = transform.position();
	float3_t size = transform.scale() * 0.5;
	float3_t rotation = glm::radians(transform.rotation());
	mat4_t rotationMatrix = glm::yawPitchRoll(-rotation.y, -rotation.z, -rotation.x);
	float_t strength = dragField.strength().at(alpha);

	for(std::uint32_t p = 0; p < particleCount; p++) {
		float3_t forceVector = sampleDragField(dragField,
			center, size,
			rotationMatrix,
			particles.globalPosition[p], particles.velocity[p], particles.size[p] * particleType.physicalSize().at(particles.life[p]));

		particles.force[p] += forceVector * strength * particleType.weight().at(particles.life[p]);
	}
}

float3_t ForceModifier::sampleAttractionField(const AttractionField& attractionField,
	const float3_t& position, float_t size,
	const float3_t& particlePosition) const {
	float_t distanceToCenter = glm::distance(position, particlePosition);
	if(distanceToCenter > size && !attractionField.infinite()) {
		return float3_t(0.0);
	}

	distanceToCenter = std::max(distanceToCenter, 0.01);

	return (position - particlePosition) / (distanceToCenter * distanceToCenter);
}
float3_t ForceModifier::sampleAccelerationField(const AccelerationField& accelerationField,
	const float3_t& position, const float3_t& size, const mat4_t& rotationMatrix, const mat4_t& directionMatrix,
	const float3_t& particlePosition) const {
	float3_t rotatedParticlePosition = position + float3_t(rotationMatrix * float4_t(particlePosition - position, 1.0));
	if(((rotatedParticlePosition.x < position.x - size.x || rotatedParticlePosition.x > position.x + size.x) ||
		(rotatedParticlePosition.y < position.y - size.y || rotatedParticlePosition.y > position.y + size.y) ||
		(rotatedParticlePosition.z < position.z - size.z || rotatedParticlePosition.z > position.z + size.z)) && !accelerationField.infinite()) {
		return float3_t(0.0);
	}

	std::int32_t gridCellX = (size.x > 0.0)
		? glm::clamp(static_cast<std::int32_t>((rotatedParticlePosition.x - (position.x - size.x)) / (size.x * 2.0) * static_cast<float_t>(accelerationField.accelerationGridSizeX())), 0, accelerationField.accelerationGridSizeX() - 1)
		: 0;
	std::int32_t gridCellY = (size.y > 0.0)
		? glm::clamp(static_cast<std::int32_t>((rotatedParticlePosition.y - (position.y - size.y)) / (size.y * 2.0) * static_cast<float_t>(accelerationField.accelerationGridSizeY())), 0, accelerationField.accelerationGridSizeY() - 1)
		: 0;
	std::int32_t gridCellZ = (size.z > 0.0)
		? glm::clamp(static_cast<std::int32_t>((rotatedParticlePosition.z - (position.z - size.z)) / (size.z * 2.0) * static_cast<float_t>(accelerationField.accelerationGridSizeZ())), 0, accelerationField.accelerationGridSizeZ() - 1)
		: 0;
	std::uint32_t gridCellIndex = static_cast<std::uint32_t>(
		gridCellZ * accelerationField.accelerationGridSizeY() * accelerationField.accelerationGridSizeX() +
		gridCellY * accelerationField.accelerationGridSizeX() +
		gridCellX);

	float3_t gridDirectionOffset = glm::radians(accelerationField.accelerationDirectionVariance().at() * accelerationField.accelerationDirectionGrid()[gridCellIndex]);
	float_t gridStrengthOffset = accelerationField.accelerationStrengthVariance().at() * accelerationField.accelerationStrengthGrid()[gridCellIndex] + 1.0;

	float3_t result = float3_t(glm::yawPitchRoll(gridDirectionOffset.y, gridDirectionOffset.z, gridDirectionOffset.x) *
		float4_t(float3_t(directionMatrix * worldUpVector4), 0.0));
	result *= gridStrengthOffset;

	return result;
}
float3_t ForceModifier::sampleVectorField(const VectorField& vectorField, const VectorFieldResource& resource,
	const float3_t& position, const float3_t& size, const mat4_t& rotationMatrix, const mat4_t& directionMatrix,
	const float3_t& particlePosition, bool& inside) const {
	float3_t rotatedParticlePosition = position + float3_t(rotationMatrix * float4_t(particlePosition - position, 1.0));
	if((rotatedParticlePosition.x < position.x - size.x || rotatedParticlePosition.x > position.x + size.x ||
		rotatedParticlePosition.y < position.y - size.y || rotatedParticlePosition.y > position.y + size.y ||
		rotatedParticlePosition.z < position.z - size.z || rotatedParticlePosition.z > position.z + size.z) && !vectorField.infinite()) {
		inside = false;
		return float3_t(0.0);
	}

	float3_t result = float3_t(0.0);
	float3_t samplePosition = (rotatedParticlePosition - position + size) / (size * 2.0);

	if(is3d) {
		switch(vectorField.vectorFieldFilter()) {
			case VectorField::Filter::none: {
				float3_t normalizedSamplePosition = float3_t(
					samplePosition.x * static_cast<float_t>(resource.field().width()),
					samplePosition.y * static_cast<float_t>(resource.field().height()),
					samplePosition.z * static_cast<float_t>(resource.field().depth()));

				result = resource.field().value(
					static_cast<std::int32_t>(normalizedSamplePosition.x),
					static_cast<std::int32_t>(normalizedSamplePosition.y),
					static_cast<std::int32_t>(normalizedSamplePosition.z),
					float3_t(0.0));
			}

			case VectorField::Filter::linear: {
				float3_t normalizedSamplePosition = float3_t(
					samplePosition.x * static_cast<float_t>(resource.field().width()),
					samplePosition.y * static_cast<float_t>(resource.field().height()),
					samplePosition.z * static_cast<float_t>(resource.field().depth()));

				float_t fractX = glm::fract(normalizedSamplePosition.x);
				float_t fractY = glm::fract(normalizedSamplePosition.y);
				float_t fractZ = glm::fract(normalizedSamplePosition.y);
				std::int32_t nextOffsetX = fractX > 0.5 ? +1 : -1;
				std::int32_t nextOffsetY = fractY > 0.5 ? +1 : -1;
				std::int32_t nextOffsetZ = fractZ > 0.5 ? +1 : -1;

				float3_t sample0 = resource.field().value(
					static_cast<std::int32_t>(normalizedSamplePosition.x),
					static_cast<std::int32_t>(normalizedSamplePosition.y),
					static_cast<std::int32_t>(normalizedSamplePosition.z),
					float3_t(0.0));
				float3_t sample1 = resource.field().value(
					static_cast<std::int32_t>(normalizedSamplePosition.x) + nextOffsetX,
					static_cast<std::int32_t>(normalizedSamplePosition.y),
					static_cast<std::int32_t>(normalizedSamplePosition.z),
					float3_t(0.0));
				float3_t sample2 = resource.field().value(
					static_cast<std::int32_t>(normalizedSamplePosition.x),
					static_cast<std::int32_t>(normalizedSamplePosition.y) + nextOffsetY,
					static_cast<std::int32_t>(normalizedSamplePosition.z),
					float3_t(0.0));
				float3_t sample3 = resource.field().value(
					static_cast<std::int32_t>(normalizedSamplePosition.x) + nextOffsetX,
					static_cast<std::int32_t>(normalizedSamplePosition.y) + nextOffsetY,
					static_cast<std::int32_t>(normalizedSamplePosition.z),
					float3_t(0.0));
				float3_t sample4 = resource.field().value(
					static_cast<std::int32_t>(normalizedSamplePosition.x),
					static_cast<std::int32_t>(normalizedSamplePosition.y),
					static_cast<std::int32_t>(normalizedSamplePosition.z) + nextOffsetZ,
					float3_t(0.0));
				float3_t sample5 = resource.field().value(
					static_cast<std::int32_t>(normalizedSamplePosition.x) + nextOffsetX,
					static_cast<std::int32_t>(normalizedSamplePosition.y),
					static_cast<std::int32_t>(normalizedSamplePosition.z) + nextOffsetZ,
					float3_t(0.0));
				float3_t sample6 = resource.field().value(
					static_cast<std::int32_t>(normalizedSamplePosition.x),
					static_cast<std::int32_t>(normalizedSamplePosition.y) + nextOffsetY,
					static_cast<std::int32_t>(normalizedSamplePosition.z) + nextOffsetZ,
					float3_t(0.0));
				float3_t sample7 = resource.field().value(
					static_cast<std::int32_t>(normalizedSamplePosition.x) + nextOffsetX,
					static_cast<std::int32_t>(normalizedSamplePosition.y) + nextOffsetY,
					static_cast<std::int32_t>(normalizedSamplePosition.z) + nextOffsetZ,
					float3_t(0.0));

				result = glm::mix(
					glm::mix(
						glm::mix(sample0, sample1, glm::abs(fractX - 0.5)),
						glm::mix(sample2, sample3, glm::abs(fractX - 0.5)),
						glm::abs(fractY - 0.5)),
					glm::mix(
						glm::mix(sample4, sample5, glm::abs(fractX - 0.5)),
						glm::mix(sample6, sample7, glm::abs(fractX - 0.5)),
						glm::abs(fractY - 0.5)),
					glm::abs(fractZ - 0.5));
			}

			default: {
				break;
			}
		}
	}
	else {
		switch(vectorField.vectorFieldFilter()) {
			case VectorField::Filter::none: {
				float3_t normalizedSamplePosition = float3_t(
					samplePosition.x * static_cast<float_t>(resource.field().width()),
					samplePosition.y * static_cast<float_t>(resource.field().height()),
					0.0);

				result = resource.field().value(
					static_cast<std::int32_t>(normalizedSamplePosition.x),
					static_cast<std::int32_t>(normalizedSamplePosition.y),
					static_cast<std::int32_t>(normalizedSamplePosition.z),
					float3_t(0.0));
			}

			case VectorField::Filter::linear: {
				float3_t normalizedSamplePosition = float3_t(
					samplePosition.x * static_cast<float_t>(resource.field().width()),
					samplePosition.y * static_cast<float_t>(resource.field().height()),
					0.0);

				float_t fractX = glm::fract(normalizedSamplePosition.x);
				float_t fractY = glm::fract(normalizedSamplePosition.y);
				std::int32_t nextOffsetX = fractX > 0.5 ? +1 : -1;
				std::int32_t nextOffsetY = fractY > 0.5 ? +1 : -1;

				float3_t sample0 = resource.field().value(
					static_cast<std::int32_t>(normalizedSamplePosition.x),
					static_cast<std::int32_t>(normalizedSamplePosition.y),
					static_cast<std::int32_t>(normalizedSamplePosition.z),
					float3_t(0.0));
				float3_t sample1 = resource.field().value(
					static_cast<std::int32_t>(normalizedSamplePosition.x) + nextOffsetX,
					static_cast<std::int32_t>(normalizedSamplePosition.y),
					static_cast<std::int32_t>(normalizedSamplePosition.z),
					float3_t(0.0));
				float3_t sample2 = resource.field().value(
					static_cast<std::int32_t>(normalizedSamplePosition.x),
					static_cast<std::int32_t>(normalizedSamplePosition.y) + nextOffsetY,
					static_cast<std::int32_t>(normalizedSamplePosition.z),
					float3_t(0.0));
				float3_t sample3 = resource.field().value(
					static_cast<std::int32_t>(normalizedSamplePosition.x) + nextOffsetX,
					static_cast<std::int32_t>(normalizedSamplePosition.y) + nextOffsetY,
					static_cast<std::int32_t>(normalizedSamplePosition.z),
					float3_t(0.0));

				result = glm::mix(
					glm::mix(sample0, sample1, glm::abs(fractX - 0.5)),
					glm::mix(sample2, sample3, glm::abs(fractX - 0.5)),
					glm::abs(fractY - 0.5));
			}

			default: {
				break;
			}
		}
	}

	inside = true;

	return float3_t(directionMatrix * float4_t(result, 0.0));
}
float3_t ForceModifier::sampleNoiseField(const NoiseField& noiseField,
	const float3_t& position, const float3_t& size, const mat4_t& rotationMatrix,
	const float3_t& particlePosition, float_t life, float_t t) const {
	float3_t rotatedParticlePosition = position + float3_t(rotationMatrix * float4_t(particlePosition - position, 1.0));
	if(((rotatedParticlePosition.x < position.x - size.x || rotatedParticlePosition.x > position.x + size.x) ||
		(rotatedParticlePosition.y < position.y - size.y || rotatedParticlePosition.y > position.y + size.y) ||
		(rotatedParticlePosition.z < position.z - size.z || rotatedParticlePosition.z > position.z + size.z)) && !noiseField.infinite()) {
		return float3_t(0.0);
	}

	float3_t samplePosition = rotatedParticlePosition - position;
	std::uint32_t octaves = static_cast<std::uint32_t>(std::max(noiseField.noiseOctaves().value(), static_cast<int_t>(0)));
	float_t frequency = noiseField.noiseFrequency().at(life);
	float_t persistence = noiseField.noisePersistence().at(life);
	float_t lacunarity = noiseField.noiseLacunarity().at(life);

	if(noiseField.noiseAnimated()) {
		float_t animationTime = noiseField.noiseAnimationTimeBase().value() + noiseField.noiseAnimationTimeScale().value() * t;

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
float3_t ForceModifier::sampleDragField(const DragField& dragField,
	const float3_t& position, const float3_t& size, const mat4_t& rotationMatrix,
	const float3_t& particlePosition, const float3_t& particleVelocity, const float3_t& particleSize) const {
	float3_t rotatedParticlePosition = position + float3_t(rotationMatrix * float4_t(particlePosition - position, 1.0));
	if(((rotatedParticlePosition.x < position.x - size.x || rotatedParticlePosition.x > position.x + size.x) ||
		(rotatedParticlePosition.y < position.y - size.y || rotatedParticlePosition.y > position.y + size.y) ||
		(rotatedParticlePosition.z < position.z - size.z || rotatedParticlePosition.z > position.z + size.z)) && !dragField.infinite()) {
		return float3_t(0.0);
	}

	float_t particleSpeed = std::max(glm::length(particleVelocity), 0.001);
	float_t particleArea = std::max(particleSize.x, std::max(particleSize.y, particleSize.z));

	return -particleVelocity / particleSpeed *
		(1.0 + (particleSpeed * particleSpeed - 1.0) * dragField.velocityInfluence().value()) *
		(1.0 + (particleArea - 1.0) * dragField.sizeInfluence().value());
}

float3_t ForceModifier::computeStaticCurlNoise2d(const float2_t& samplePosition, std::uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const {
	const float_t epsilon = 1.0e-4;

	float_t x1 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + epsilon,
		samplePosition.y);
	float_t x2 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x - epsilon,
		samplePosition.y);

	float_t y1 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y + epsilon);
	float_t y2 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y - epsilon);

	return float3_t(
		(x1 - x2) / epsilon * 0.5,
		(y1 - y2) / epsilon * 0.5,
		0.0);
}
float3_t ForceModifier::computeStaticCurlNoise3d(const float3_t& samplePosition, std::uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const {
	const float_t epsilon = 1.0e-4;
	const float_t offset = 1000.0;

	float_t x1 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + epsilon,
		samplePosition.y,
		samplePosition.z);
	float_t x2 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x - epsilon,
		samplePosition.y,
		samplePosition.z);

	float_t y1 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y + epsilon,
		samplePosition.z);
	float_t y2 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y - epsilon,
		samplePosition.z);

	float_t z1 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y,
		samplePosition.z + epsilon);
	float_t z2 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y,
		samplePosition.z - epsilon);

	float3_t noiseGradient1 = float3_t(
		(x1 - x2) / epsilon * 0.5,
		(y1 - y2) / epsilon * 0.5,
		(z1 - z2) / epsilon * 0.5);

	x1 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset + epsilon,
		samplePosition.y + offset,
		samplePosition.z + offset);
	x2 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset - epsilon,
		samplePosition.y + offset,
		samplePosition.z + offset);

	y1 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset,
		samplePosition.y + offset + epsilon,
		samplePosition.z + offset);
	y2 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset,
		samplePosition.y + offset - epsilon,
		samplePosition.z + offset);

	z1 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset,
		samplePosition.y + offset,
		samplePosition.z + offset + epsilon);
	z2 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset,
		samplePosition.y + offset,
		samplePosition.z + offset - epsilon);

	float3_t noiseGradient2 = float3_t(
		(x1 - x2) / epsilon * 0.5,
		(y1 - y2) / epsilon * 0.5,
		(z1 - z2) / epsilon * 0.5);

	return glm::normalize(glm::cross(
		glm::normalize(noiseGradient1),
		glm::normalize(noiseGradient2)));
}
float3_t ForceModifier::computeAnimatedCurlNoise2d(const float2_t& samplePosition, float_t animationTime, std::uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const {
	const float_t epsilon = 1.0e-4;

	float_t x1 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + epsilon,
		samplePosition.y,
		animationTime);
	float_t x2 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x - epsilon,
		samplePosition.y,
		animationTime);

	float_t y1 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y + epsilon,
		animationTime);
	float_t y2 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y - epsilon,
		animationTime);

	return float3_t(
		(x1 - x2) / epsilon * 0.5,
		(y1 - y2) / epsilon * 0.5,
		0.0);
}
float3_t ForceModifier::computeAnimatedCurlNoise3d(const float3_t& samplePosition, float_t animationTime, std::uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const {
	const float_t epsilon = 1.0e-4;
	const float_t offset = 1000.0;

	float_t x1 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + epsilon,
		samplePosition.y,
		samplePosition.z,
		animationTime);
	float_t x2 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x - epsilon,
		samplePosition.y,
		samplePosition.z,
		animationTime);

	float_t y1 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y + epsilon,
		samplePosition.z,
		animationTime);
	float_t y2 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y - epsilon,
		samplePosition.z,
		animationTime);

	float_t z1 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y,
		samplePosition.z + epsilon,
		animationTime);
	float_t z2 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x,
		samplePosition.y,
		samplePosition.z - epsilon,
		animationTime);

	float3_t noiseGradient1 = float3_t(
		(x1 - x2) / epsilon * 0.5,
		(y1 - y2) / epsilon * 0.5,
		(z1 - z2) / epsilon * 0.5);

	x1 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset + epsilon,
		samplePosition.y + offset,
		samplePosition.z + offset,
		animationTime);
	x2 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset - epsilon,
		samplePosition.y + offset,
		samplePosition.z + offset,
		animationTime);

	y1 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset,
		samplePosition.y + offset + epsilon,
		samplePosition.z + offset,
		animationTime);
	y2 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset,
		samplePosition.y + offset - epsilon,
		samplePosition.z + offset,
		animationTime);

	z1 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset,
		samplePosition.y + offset,
		samplePosition.z + offset + epsilon,
		animationTime);
	z2 = noise::simplexFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition.x + offset,
		samplePosition.y + offset,
		samplePosition.z + offset - epsilon,
		animationTime);

	float3_t noiseGradient2 = float3_t(
		(x1 - x2) / epsilon * 0.5,
		(y1 - y2) / epsilon * 0.5,
		(z1 - z2) / epsilon * 0.5);

	return glm::normalize(glm::cross(
		glm::normalize(noiseGradient1),
		glm::normalize(noiseGradient2)));
}
}