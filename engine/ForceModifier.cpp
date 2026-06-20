#include "ForceModifier.h"
#include "../common/Curve.h"
#include "../common/Coordinates.h"
#include "../common/Transform.h"
#include "../math/Common.h"
#include "../math/Geometry.h"
#include "../math/Trigonometry.h"
#include "../math/Transformation.h"
#include "../math/Interpolation.h"
#include "../math/Noise.h"
#include "../effect/SceneGraph.h"
#include "../effect/ParticleType.h"
#include <cmath>
#include <iterator>
#include <algorithm>

namespace pixelpart {
void ForceModifier::apply(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
	const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const {
	const ParticleType& particleType = effect->particleTypes().at(particleTypeId);

	for(const AttractionField& field : modifierAttractionFields) {
		if(field.exclusionSet().count(particleType.id()) != 0) {
			continue;
		}

		applyForce(particles, particleCount, runtimeContext, particleType, field, effect->sceneGraph());
	}

	for(const AccelerationField& field : modifierAccelerationFields) {
		if(field.exclusionSet().count(particleType.id()) != 0) {
			continue;
		}

		applyForce(particles, particleCount, runtimeContext, particleType, field, effect->sceneGraph());
	}

	for(const VectorField& field : modifierVectorFields) {
		if(field.exclusionSet().count(particleType.id()) != 0) {
			continue;
		}

		applyForce(particles, particleCount, runtimeContext, particleType, field, effect->sceneGraph());
	}

	for(const NoiseField& field : modifierNoiseFields) {
		if(field.exclusionSet().count(particleType.id()) != 0) {
			continue;
		}

		applyForce(particles, particleCount, runtimeContext, particleType, field, effect->sceneGraph());
	}

	for(const DragField& field : modifierDragFields) {
		if(field.exclusionSet().count(particleType.id()) != 0) {
			continue;
		}

		applyForce(particles, particleCount, runtimeContext, particleType, field, effect->sceneGraph());
	}
}

void ForceModifier::reset(const Effect* effect, EffectRuntimeContext runtimeContext) {
	modifierEffectResources = &effect->resources();
	modifierEffect3d = effect->is3d();

	modifierAttractionFields.clear();
	modifierAccelerationFields.clear();
	modifierVectorFields.clear();
	modifierNoiseFields.clear();
	modifierDragFields.clear();

	for(const ForceField* forceField : effect->sceneGraph().nodesWithType<ForceField>()) {
		if(!forceField->active(runtimeContext)) {
			continue;
		}

		const AttractionField* attractionField = dynamic_cast<const AttractionField*>(forceField);
		const AccelerationField* accelerationField = dynamic_cast<const AccelerationField*>(forceField);
		const VectorField* vectorField = dynamic_cast<const VectorField*>(forceField);
		const NoiseField* noiseField = dynamic_cast<const NoiseField*>(forceField);
		const DragField* dragField = dynamic_cast<const DragField*>(forceField);

		if(attractionField) {
			modifierAttractionFields.emplace_back(*attractionField);
		}
		else if(accelerationField) {
			modifierAccelerationFields.emplace_back(*accelerationField);
		}
		else if(vectorField) {
			modifierVectorFields.emplace_back(*vectorField);
		}
		else if(noiseField) {
			modifierNoiseFields.emplace_back(*noiseField);
		}
		else if(dragField) {
			modifierDragFields.emplace_back(*dragField);
		}
	}
}

void ForceModifier::applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const EffectRuntimeContext& runtimeContext,
	const ParticleType& particleType, const AttractionField& attractionField, const SceneGraph& sceneGraph) const {
	float_t fieldLife = attractionField.life(runtimeContext);
	Transform fieldTransform = sceneGraph.globalTransform(attractionField.id(), runtimeContext);
	float3_t fieldPosition = fieldTransform.position();
	float3_t fieldSize = fieldTransform.scale() * 0.5;
	float_t fieldStrength = attractionField.strength().at(fieldLife);
	bool fieldInfinite = attractionField.infinite();

	const Curve<float_t>& particleWeightCurve = particleType.weight().resultCurve();

	for(std::uint32_t p = 0; p < particleCount; p++) {
		float3_t particleToCenter = fieldPosition - particles.globalPosition[p];
		float_t distanceToCenter = math::length(particleToCenter);
		if(distanceToCenter > fieldSize.x && !fieldInfinite) {
			continue;
		}

		distanceToCenter = std::max(distanceToCenter, 0.01);

		float3_t forceVector = particleToCenter / (distanceToCenter * distanceToCenter);
		particles.force[p] += forceVector * fieldStrength * particleWeightCurve.at(particles.life[p]);
	}
}
void ForceModifier::applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const EffectRuntimeContext& runtimeContext,
	const ParticleType& particleType, const AccelerationField& accelerationField, const SceneGraph& sceneGraph) const {
	float_t fieldLife = accelerationField.life(runtimeContext);
	Transform fieldTransform = sceneGraph.globalTransform(accelerationField.id(), runtimeContext);
	float3_t fieldPosition = fieldTransform.position();
	float3_t fieldSize = fieldTransform.scale() * 0.5;
	float3_t fieldRotation = math::radians(fieldTransform.rotation());
	matrix4_t fieldRotationMatrix = math::yawPitchRollRotationMatrix(fieldRotation.y, fieldRotation.z, fieldRotation.x);
	matrix4_t fieldInverseRotationMatrix = math::yawPitchRollRotationMatrix(-fieldRotation.y, -fieldRotation.z, -fieldRotation.x);
	float_t fieldStrength = accelerationField.strength().at(fieldLife);
	bool fieldInfinite = accelerationField.infinite();

	float3_t accelerationDirection = math::radians(accelerationField.accelerationDirection().at(fieldLife));
	matrix4_t accelerationDirectionMatrix = math::yawPitchRollRotationMatrix(accelerationDirection.y, accelerationDirection.z, accelerationDirection.x);
	float_t accelerationStrengthVariance = accelerationField.accelerationStrengthVariance().at(fieldLife);
	float_t accelerationDirectionVariance = accelerationField.accelerationDirectionVariance().at(fieldLife);
	std::int32_t accelerationGridSizeX = accelerationField.accelerationGridSizeX();
	std::int32_t accelerationGridSizeY = accelerationField.accelerationGridSizeY();
	std::int32_t accelerationGridSizeZ = accelerationField.accelerationGridSizeZ();
	const std::vector<float3_t>& accelerationDirectionGrid = accelerationField.accelerationDirectionGrid();
	const std::vector<float_t>& accelerationStrengthGrid = accelerationField.accelerationStrengthGrid();

	const Curve<float_t>& particleWeightCurve = particleType.weight().resultCurve();

	for(std::uint32_t p = 0; p < particleCount; p++) {
		float3_t localParticlePosition = float3_t(fieldInverseRotationMatrix * float4_t(particles.globalPosition[p] - fieldPosition, 1.0));
		if(((localParticlePosition.x < -fieldSize.x || localParticlePosition.x > fieldSize.x) ||
			(localParticlePosition.y < -fieldSize.y || localParticlePosition.y > fieldSize.y) ||
			(localParticlePosition.z < -fieldSize.z || localParticlePosition.z > fieldSize.z)) && !fieldInfinite) {
			continue;
		}

		std::int32_t gridCellX = fieldSize.x > 0.0
			? std::clamp(static_cast<std::int32_t>((localParticlePosition.x + fieldSize.x) / (fieldSize.x * 2.0) * static_cast<float_t>(accelerationGridSizeX)), 0, accelerationGridSizeX - 1)
			: 0;
		std::int32_t gridCellY = fieldSize.y > 0.0
			? std::clamp(static_cast<std::int32_t>((localParticlePosition.y + fieldSize.y) / (fieldSize.y * 2.0) * static_cast<float_t>(accelerationGridSizeY)), 0, accelerationGridSizeY - 1)
			: 0;
		std::int32_t gridCellZ = fieldSize.z > 0.0
			? std::clamp(static_cast<std::int32_t>((localParticlePosition.z + fieldSize.z) / (fieldSize.z * 2.0) * static_cast<float_t>(accelerationGridSizeZ)), 0, accelerationGridSizeZ - 1)
			: 0;
		std::uint32_t gridCellIndex = static_cast<std::uint32_t>(
			gridCellZ * accelerationGridSizeY * accelerationGridSizeX +
			gridCellY * accelerationGridSizeX +
			gridCellX);

		float3_t gridDirectionOffset = math::radians(accelerationDirectionVariance * accelerationDirectionGrid[gridCellIndex]);
		float_t gridStrengthOffset = accelerationStrengthVariance * accelerationStrengthGrid[gridCellIndex] + 1.0;
		matrix4_t directionOffsetMatrix = math::yawPitchRollRotationMatrix(gridDirectionOffset.y, gridDirectionOffset.z, gridDirectionOffset.x);

		float3_t forceVector = float3_t(fieldRotationMatrix * directionOffsetMatrix * accelerationDirectionMatrix * worldUpVector4);

		particles.force[p] += forceVector * gridStrengthOffset * fieldStrength * particleWeightCurve.at(particles.life[p]);
	}
}
void ForceModifier::applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const EffectRuntimeContext& runtimeContext,
	const ParticleType& particleType, const VectorField& vectorField, const SceneGraph& sceneGraph) const {
	if(modifierEffectResources == nullptr || modifierEffectResources->vectorFields().count(vectorField.vectorFieldResourceId()) == 0) {
		return;
	}

	float_t fieldLife = vectorField.life(runtimeContext);
	Transform fieldTransform = sceneGraph.globalTransform(vectorField.id(), runtimeContext);
	float3_t fieldPosition = fieldTransform.position();
	float3_t fieldSize = fieldTransform.scale() * 0.5;
	float3_t fieldRotation = math::radians(fieldTransform.rotation());
	matrix4_t fieldRotationMatrix = math::yawPitchRollRotationMatrix(fieldRotation.y, fieldRotation.z, fieldRotation.x);
	matrix4_t fieldInverseRotationMatrix = math::yawPitchRollRotationMatrix(-fieldRotation.y, -fieldRotation.z, -fieldRotation.x);
	float_t fieldStrength = vectorField.strength().at(fieldLife);
	bool fieldInfinite = vectorField.infinite();

	const VectorFieldResource& vectorFieldResource = modifierEffectResources->vectorFields().at(vectorField.vectorFieldResourceId());
	const Grid3d<float3_t>& vectorFieldGrid = vectorFieldResource.field();
	float_t vectorFieldTightness = std::clamp(vectorField.tightness().at(fieldLife), 0.0, 1.0);

	const Curve<float_t>& particleWeightCurve = particleType.weight().resultCurve();

	for(std::uint32_t p = 0; p < particleCount; p++) {
		float3_t localParticlePosition = float3_t(fieldInverseRotationMatrix * float4_t(particles.globalPosition[p] - fieldPosition, 1.0));
		if((localParticlePosition.x < -fieldSize.x || localParticlePosition.x > fieldSize.x ||
			localParticlePosition.y < -fieldSize.y || localParticlePosition.y > fieldSize.y ||
			localParticlePosition.z < -fieldSize.z || localParticlePosition.z > fieldSize.z) && !fieldInfinite) {
			continue;
		}

		float3_t forceVector = float3_t(0.0);
		float3_t samplePosition = (localParticlePosition + fieldSize) / (fieldSize * 2.0);

		if(modifierEffect3d) {
			switch(vectorField.vectorFieldFilter()) {
				case VectorField::Filter::none: {
					float3_t normalizedSamplePosition = float3_t(
						samplePosition.x * static_cast<float_t>(vectorFieldGrid.width()),
						samplePosition.y * static_cast<float_t>(vectorFieldGrid.height()),
						samplePosition.z * static_cast<float_t>(vectorFieldGrid.depth()));

					forceVector = vectorFieldGrid.value(
						static_cast<std::int32_t>(normalizedSamplePosition.x),
						static_cast<std::int32_t>(normalizedSamplePosition.y),
						static_cast<std::int32_t>(normalizedSamplePosition.z),
						float3_t(0.0));

					break;
				}

				case VectorField::Filter::linear: {
					float3_t normalizedSamplePosition = float3_t(
						samplePosition.x * static_cast<float_t>(vectorFieldGrid.width()),
						samplePosition.y * static_cast<float_t>(vectorFieldGrid.height()),
						samplePosition.z * static_cast<float_t>(vectorFieldGrid.depth()));

					float_t fractX = math::fract(normalizedSamplePosition.x);
					float_t fractY = math::fract(normalizedSamplePosition.y);
					float_t fractZ = math::fract(normalizedSamplePosition.y);
					std::int32_t nextOffsetX = fractX > 0.5 ? +1 : -1;
					std::int32_t nextOffsetY = fractY > 0.5 ? +1 : -1;
					std::int32_t nextOffsetZ = fractZ > 0.5 ? +1 : -1;

					float3_t sample0 = vectorFieldGrid.value(
						static_cast<std::int32_t>(normalizedSamplePosition.x),
						static_cast<std::int32_t>(normalizedSamplePosition.y),
						static_cast<std::int32_t>(normalizedSamplePosition.z),
						float3_t(0.0));
					float3_t sample1 = vectorFieldGrid.value(
						static_cast<std::int32_t>(normalizedSamplePosition.x) + nextOffsetX,
						static_cast<std::int32_t>(normalizedSamplePosition.y),
						static_cast<std::int32_t>(normalizedSamplePosition.z),
						float3_t(0.0));
					float3_t sample2 = vectorFieldGrid.value(
						static_cast<std::int32_t>(normalizedSamplePosition.x),
						static_cast<std::int32_t>(normalizedSamplePosition.y) + nextOffsetY,
						static_cast<std::int32_t>(normalizedSamplePosition.z),
						float3_t(0.0));
					float3_t sample3 = vectorFieldGrid.value(
						static_cast<std::int32_t>(normalizedSamplePosition.x) + nextOffsetX,
						static_cast<std::int32_t>(normalizedSamplePosition.y) + nextOffsetY,
						static_cast<std::int32_t>(normalizedSamplePosition.z),
						float3_t(0.0));
					float3_t sample4 = vectorFieldGrid.value(
						static_cast<std::int32_t>(normalizedSamplePosition.x),
						static_cast<std::int32_t>(normalizedSamplePosition.y),
						static_cast<std::int32_t>(normalizedSamplePosition.z) + nextOffsetZ,
						float3_t(0.0));
					float3_t sample5 = vectorFieldGrid.value(
						static_cast<std::int32_t>(normalizedSamplePosition.x) + nextOffsetX,
						static_cast<std::int32_t>(normalizedSamplePosition.y),
						static_cast<std::int32_t>(normalizedSamplePosition.z) + nextOffsetZ,
						float3_t(0.0));
					float3_t sample6 = vectorFieldGrid.value(
						static_cast<std::int32_t>(normalizedSamplePosition.x),
						static_cast<std::int32_t>(normalizedSamplePosition.y) + nextOffsetY,
						static_cast<std::int32_t>(normalizedSamplePosition.z) + nextOffsetZ,
						float3_t(0.0));
					float3_t sample7 = vectorFieldGrid.value(
						static_cast<std::int32_t>(normalizedSamplePosition.x) + nextOffsetX,
						static_cast<std::int32_t>(normalizedSamplePosition.y) + nextOffsetY,
						static_cast<std::int32_t>(normalizedSamplePosition.z) + nextOffsetZ,
						float3_t(0.0));

					forceVector = math::linearInterpolation(
						math::linearInterpolation(
							math::linearInterpolation(sample0, sample1, std::abs(fractX - 0.5)),
							math::linearInterpolation(sample2, sample3, std::abs(fractX - 0.5)),
							std::abs(fractY - 0.5)),
						math::linearInterpolation(
							math::linearInterpolation(sample4, sample5, std::abs(fractX - 0.5)),
							math::linearInterpolation(sample6, sample7, std::abs(fractX - 0.5)),
							std::abs(fractY - 0.5)),
						std::abs(fractZ - 0.5));

					break;
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
						samplePosition.x * static_cast<float_t>(vectorFieldGrid.width()),
						samplePosition.y * static_cast<float_t>(vectorFieldGrid.height()),
						0.0);

					forceVector = vectorFieldGrid.value(
						static_cast<std::int32_t>(normalizedSamplePosition.x),
						static_cast<std::int32_t>(normalizedSamplePosition.y),
						static_cast<std::int32_t>(normalizedSamplePosition.z),
						float3_t(0.0));

					break;
				}

				case VectorField::Filter::linear: {
					float3_t normalizedSamplePosition = float3_t(
						samplePosition.x * static_cast<float_t>(vectorFieldGrid.width()),
						samplePosition.y * static_cast<float_t>(vectorFieldGrid.height()),
						0.0);

					float_t fractX = math::fract(normalizedSamplePosition.x);
					float_t fractY = math::fract(normalizedSamplePosition.y);
					std::int32_t nextOffsetX = fractX > 0.5 ? +1 : -1;
					std::int32_t nextOffsetY = fractY > 0.5 ? +1 : -1;

					float3_t sample0 = vectorFieldGrid.value(
						static_cast<std::int32_t>(normalizedSamplePosition.x),
						static_cast<std::int32_t>(normalizedSamplePosition.y),
						static_cast<std::int32_t>(normalizedSamplePosition.z),
						float3_t(0.0));
					float3_t sample1 = vectorFieldGrid.value(
						static_cast<std::int32_t>(normalizedSamplePosition.x) + nextOffsetX,
						static_cast<std::int32_t>(normalizedSamplePosition.y),
						static_cast<std::int32_t>(normalizedSamplePosition.z),
						float3_t(0.0));
					float3_t sample2 = vectorFieldGrid.value(
						static_cast<std::int32_t>(normalizedSamplePosition.x),
						static_cast<std::int32_t>(normalizedSamplePosition.y) + nextOffsetY,
						static_cast<std::int32_t>(normalizedSamplePosition.z),
						float3_t(0.0));
					float3_t sample3 = vectorFieldGrid.value(
						static_cast<std::int32_t>(normalizedSamplePosition.x) + nextOffsetX,
						static_cast<std::int32_t>(normalizedSamplePosition.y) + nextOffsetY,
						static_cast<std::int32_t>(normalizedSamplePosition.z),
						float3_t(0.0));

					forceVector = math::linearInterpolation(
						math::linearInterpolation(sample0, sample1, std::abs(fractX - 0.5)),
						math::linearInterpolation(sample2, sample3, std::abs(fractX - 0.5)),
						std::abs(fractY - 0.5));

					break;
				}

				default: {
					break;
				}
			}
		}

		forceVector = float3_t(fieldRotationMatrix * float4_t(forceVector, 0.0));
		forceVector *= fieldStrength * particleWeightCurve.at(particles.life[p]);

		particles.force[p] += forceVector * (1.0 - vectorFieldTightness);
		particles.velocity[p] *= 1.0 - vectorFieldTightness;
		particles.velocity[p] += forceVector * vectorFieldTightness;
	}
}
void ForceModifier::applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const EffectRuntimeContext& runtimeContext,
	const ParticleType& particleType, const NoiseField& noiseField, const SceneGraph& sceneGraph) const {
	float_t fieldLife = noiseField.life(runtimeContext);
	Transform fieldTransform = sceneGraph.globalTransform(noiseField.id(), runtimeContext);
	float3_t fieldPosition = fieldTransform.position();
	float3_t fieldSize = fieldTransform.scale() * 0.5;
	float3_t fieldRotation = math::radians(fieldTransform.rotation());
	matrix4_t fieldInverseRotationMatrix = math::yawPitchRollRotationMatrix(-fieldRotation.y, -fieldRotation.z, -fieldRotation.x);
	float_t fieldStrength = noiseField.strength().at(fieldLife);
	bool fieldInfinite = noiseField.infinite();

	std::uint32_t noiseOctaves = static_cast<std::uint32_t>(std::max(noiseField.noiseOctaves().value(), static_cast<int_t>(0)));
	float_t noiseFrequency = noiseField.noiseFrequency().at(fieldLife);
	float_t noisePersistence = noiseField.noisePersistence().at(fieldLife);
	float_t noiseLacunarity = noiseField.noiseLacunarity().at(fieldLife);
	bool noiseAnimated = noiseField.noiseAnimated();
	float_t noiseAnimationTime = noiseField.noiseAnimationTimeBase().value() +
		noiseField.noiseAnimationTimeScale().value() * runtimeContext.time();

	const Curve<float_t>& particleWeightCurve = particleType.weight().resultCurve();

	for(std::uint32_t p = 0; p < particleCount; p++) {
		float3_t localParticlePosition = float3_t(fieldInverseRotationMatrix * float4_t(particles.globalPosition[p] - fieldPosition, 1.0));
		if(((localParticlePosition.x < -fieldSize.x || localParticlePosition.x > fieldSize.x) ||
			(localParticlePosition.y < -fieldSize.y || localParticlePosition.y > fieldSize.y) ||
			(localParticlePosition.z < -fieldSize.z || localParticlePosition.z > fieldSize.z)) && !fieldInfinite) {
			continue;
		}

		float3_t forceVector;
		if(noiseAnimated) {
			forceVector = modifierEffect3d
				? computeAnimatedCurlNoise3d(localParticlePosition, noiseAnimationTime, noiseOctaves, noiseFrequency, noisePersistence, noiseLacunarity)
				: computeAnimatedCurlNoise2d(float2_t(localParticlePosition), noiseAnimationTime, noiseOctaves, noiseFrequency, noisePersistence, noiseLacunarity);
		}
		else {
			forceVector = modifierEffect3d
				? computeStaticCurlNoise3d(localParticlePosition, noiseOctaves, noiseFrequency, noisePersistence, noiseLacunarity)
				: computeStaticCurlNoise2d(float2_t(localParticlePosition), noiseOctaves, noiseFrequency, noisePersistence, noiseLacunarity);
		}

		particles.force[p] += forceVector * fieldStrength * particleWeightCurve.at(particles.life[p]);
	}
}
void ForceModifier::applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const EffectRuntimeContext& runtimeContext,
	const ParticleType& particleType, const DragField& dragField, const SceneGraph& sceneGraph) const {
	float_t fieldLife = dragField.life(runtimeContext);
	Transform fieldTransform = sceneGraph.globalTransform(dragField.id(), runtimeContext);
	float3_t fieldPosition = fieldTransform.position();
	float3_t fieldSize = fieldTransform.scale() * 0.5;
	float3_t fieldRotation = math::radians(fieldTransform.rotation());
	matrix4_t fieldInverseRotationMatrix = math::yawPitchRollRotationMatrix(-fieldRotation.y, -fieldRotation.z, -fieldRotation.x);
	float_t fieldStrength = dragField.strength().at(fieldLife);
	bool fieldInfinite = dragField.infinite();

	float_t dragVelocityInfluence = dragField.velocityInfluence().value();
	float_t dragSizeInfluence = dragField.sizeInfluence().value();

	const Curve<float_t>& particleWeightCurve = particleType.weight().resultCurve();
	const Curve<float_t>& particlePhysicalSizeCurve = particleType.physicalSize().resultCurve();

	for(std::uint32_t p = 0; p < particleCount; p++) {
		float3_t localParticlePosition = float3_t(fieldInverseRotationMatrix * float4_t(particles.globalPosition[p] - fieldPosition, 1.0));
		if(((localParticlePosition.x < -fieldSize.x || localParticlePosition.x > fieldSize.x) ||
			(localParticlePosition.y < -fieldSize.y || localParticlePosition.y > fieldSize.y) ||
			(localParticlePosition.z < -fieldSize.z || localParticlePosition.z > fieldSize.z)) && !fieldInfinite) {
			continue;
		}

		float3_t particleSize = particles.size[p] * particlePhysicalSizeCurve.at(particles.life[p]);
		float_t particleSpeed = std::max(math::length(particles.velocity[p]), 0.001);
		float_t particleArea = std::max(particleSize.x, std::max(particleSize.y, particleSize.z));

		float3_t forceVector = -particles.velocity[p] / particleSpeed *
			(1.0 + (particleSpeed * particleSpeed - 1.0) * dragVelocityInfluence) *
			(1.0 + (particleArea - 1.0) * dragSizeInfluence);

		particles.force[p] += forceVector * fieldStrength * particleWeightCurve.at(particles.life[p]);
	}
}

float3_t ForceModifier::computeStaticCurlNoise2d(const float2_t& samplePosition, std::uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const {
	const float_t epsilon = 1.0e-4;

	float_t x1 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition + float2_t(epsilon, 0));
	float_t x2 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition - float2_t(epsilon, 0));

	float_t y1 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition + float2_t(0, epsilon));
	float_t y2 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition - float2_t(0, epsilon));

	return float3_t(
		(x1 - x2) / epsilon * 0.5,
		(y1 - y2) / epsilon * 0.5,
		0.0);
}
float3_t ForceModifier::computeStaticCurlNoise3d(const float3_t& samplePosition, std::uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const {
	const float_t epsilon = 1.0e-4;
	const float3_t offset = float3_t(1000.0);

	float_t x1 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition + float3_t(epsilon, 0, 0));
	float_t x2 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition - float3_t(epsilon, 0, 0));

	float_t y1 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition + float3_t(0, epsilon, 0));
	float_t y2 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition - float3_t(0, epsilon, 0));

	float_t z1 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition + float3_t(0, 0, epsilon));
	float_t z2 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition - float3_t(0, 0, epsilon));

	float3_t noiseGradient1 = float3_t(
		(x1 - x2) / epsilon * 0.5,
		(y1 - y2) / epsilon * 0.5,
		(z1 - z2) / epsilon * 0.5);

	x1 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition + offset + float3_t(epsilon, 0, 0));
	x2 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition + offset - float3_t(epsilon, 0, 0));

	y1 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition + offset + float3_t(0, epsilon, 0));
	y2 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition + offset - float3_t(0, epsilon, 0));

	z1 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition + offset + float3_t(0, 0, epsilon));
	z2 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		samplePosition + offset - float3_t(0, 0, epsilon));

	float3_t noiseGradient2 = float3_t(
		(x1 - x2) / epsilon * 0.5,
		(y1 - y2) / epsilon * 0.5,
		(z1 - z2) / epsilon * 0.5);

	return math::normalize(math::cross(
		math::normalize(noiseGradient1),
		math::normalize(noiseGradient2)));
}
float3_t ForceModifier::computeAnimatedCurlNoise2d(const float2_t& samplePosition, float_t animationTime, std::uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const {
	const float_t epsilon = 1.0e-4;

	float_t x1 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		float3_t(samplePosition + float2_t(epsilon, 0), animationTime));
	float_t x2 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		float3_t(samplePosition - float2_t(epsilon, 0), animationTime));

	float_t y1 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		float3_t(samplePosition + float2_t(0, epsilon), animationTime));
	float_t y2 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		float3_t(samplePosition - float2_t(0, epsilon), animationTime));

	return float3_t(
		(x1 - x2) / epsilon * 0.5,
		(y1 - y2) / epsilon * 0.5,
		0.0);
}
float3_t ForceModifier::computeAnimatedCurlNoise3d(const float3_t& samplePosition, float_t animationTime, std::uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const {
	const float_t epsilon = 1.0e-4;
	const float3_t offset = float3_t(1000.0);

	float_t x1 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		float4_t(samplePosition + float3_t(epsilon, 0, 0), animationTime));
	float_t x2 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		float4_t(samplePosition - float3_t(epsilon, 0, 0), animationTime));

	float_t y1 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		float4_t(samplePosition + float3_t(0, epsilon, 0), animationTime));
	float_t y2 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		float4_t(samplePosition - float3_t(0, epsilon, 0), animationTime));

	float_t z1 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		float4_t(samplePosition + float3_t(0, 0, epsilon), animationTime));
	float_t z2 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		float4_t(samplePosition - float3_t(0, 0, epsilon), animationTime));

	float3_t noiseGradient1 = float3_t(
		(x1 - x2) / epsilon * 0.5,
		(y1 - y2) / epsilon * 0.5,
		(z1 - z2) / epsilon * 0.5);

	x1 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		float4_t(samplePosition + offset + float3_t(epsilon, 0, 0), animationTime));
	x2 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		float4_t(samplePosition + offset - float3_t(epsilon, 0, 0), animationTime));

	y1 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		float4_t(samplePosition + offset + float3_t(0, epsilon, 0), animationTime));
	y2 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		float4_t(samplePosition + offset - float3_t(0, epsilon, 0), animationTime));

	z1 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		float4_t(samplePosition + offset + float3_t(0, 0, epsilon), animationTime));
	z2 = math::simplexNoiseFBM(
		octaves, frequency, persistence, lacunarity,
		float4_t(samplePosition + offset - float3_t(0, 0, epsilon), animationTime));

	float3_t noiseGradient2 = float3_t(
		(x1 - x2) / epsilon * 0.5,
		(y1 - y2) / epsilon * 0.5,
		(z1 - z2) / epsilon * 0.5);

	return math::normalize(math::cross(
		math::normalize(noiseGradient1),
		math::normalize(noiseGradient2)));
}
}
