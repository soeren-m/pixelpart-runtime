#include "ForceModifier.h"
#include "../effect/SceneGraph.h"
#include "../effect/ParticleType.h"
#include "../effect/Coordinates.h"
#include "../effect/Transform.h"
#include "../effect/Curve.h"
#include "../math/Common.h"
#include "../math/MatrixCommon.h"
#include "../math/Geometry.h"
#include "../math/Trigonometry.h"
#include "../math/Transformation.h"
#include "../math/Interpolation.h"
#include "../math/Noise.h"
#include <cmath>
#include <iterator>
#include <algorithm>

namespace pixelpart {
void ForceModifier::apply(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
	const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const {
	const ParticleType& particleType = effect->particleTypes().at(particleTypeId);

	matrix4_t globalEmitterTransform = effect->sceneGraph().globalTransform(particleEmitterId, runtimeContext).matrix();
	matrix3_t globalToSimulationSpaceTransform = particleType.simulationSpace() == ParticleSimulationSpace::local
		? matrix3_t(math::inverse(math::normalizeTransformationMatrix(globalEmitterTransform)))
		: matrix3_t(1.0);

	for(const AttractionField& field : modifierAttractionFields) {
		if(field.exclusionSet().count(particleType.id()) != 0) {
			continue;
		}

		applyForce(particles, particleCount,
			runtimeContext, particleType, particleEmitterId, field, effect->sceneGraph(),
			globalToSimulationSpaceTransform);
	}

	for(const AccelerationFieldData& field : modifierAccelerationFields) {
		if(field.forceField.exclusionSet().count(particleType.id()) != 0) {
			continue;
		}

		applyForce(particles, particleCount,
			runtimeContext, particleType, particleEmitterId, field, effect->sceneGraph(),
			globalToSimulationSpaceTransform);
	}

	for(const VectorField& field : modifierVectorFields) {
		if(field.exclusionSet().count(particleType.id()) != 0) {
			continue;
		}

		applyForce(particles, particleCount,
			runtimeContext, particleType, particleEmitterId, field, effect->sceneGraph(),
			globalToSimulationSpaceTransform);
	}

	for(const NoiseField& field : modifierNoiseFields) {
		if(field.exclusionSet().count(particleType.id()) != 0) {
			continue;
		}

		applyForce(particles, particleCount,
			runtimeContext, particleType, particleEmitterId, field, effect->sceneGraph(),
			globalToSimulationSpaceTransform);
	}

	for(const DragField& field : modifierDragFields) {
		if(field.exclusionSet().count(particleType.id()) != 0) {
			continue;
		}

		applyForce(particles, particleCount,
			runtimeContext, particleType, particleEmitterId, field, effect->sceneGraph(),
			globalToSimulationSpaceTransform);
	}

	for(const VortexField& field : modifierVortexFields) {
		if(field.exclusionSet().count(particleType.id()) != 0) {
			continue;
		}

		applyForce(particles, particleCount,
			runtimeContext, particleType, particleEmitterId, field, effect->sceneGraph(),
			globalToSimulationSpaceTransform);
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
	modifierVortexFields.clear();

	for(const ForceField* forceField : effect->sceneGraph().nodesWithType<ForceField>()) {
		if(!forceField->active(runtimeContext)) {
			continue;
		}

		const AttractionField* attractionField = dynamic_cast<const AttractionField*>(forceField);
		const AccelerationField* accelerationField = dynamic_cast<const AccelerationField*>(forceField);
		const VectorField* vectorField = dynamic_cast<const VectorField*>(forceField);
		const NoiseField* noiseField = dynamic_cast<const NoiseField*>(forceField);
		const DragField* dragField = dynamic_cast<const DragField*>(forceField);
		const VortexField* vortexField = dynamic_cast<const VortexField*>(forceField);

		float_t life = forceField->life(runtimeContext);

		if(attractionField) {
			modifierAttractionFields.emplace_back(*attractionField);
		}
		else if(accelerationField) {
			const std::vector<float_t>& accelerationStrengthGrid = accelerationField->accelerationStrengthGrid();
			const std::vector<float3_t>& accelerationDirectionGrid = accelerationField->accelerationDirectionGrid();

			AccelerationFieldData fieldData;
			fieldData.forceField = *accelerationField;
			fieldData.strengthGrid.resize(accelerationStrengthGrid.size());
			fieldData.directionMatrixGrid.resize(accelerationDirectionGrid.size());

			float_t accelerationStrengthVariance = accelerationField->accelerationStrengthVariance().at(life);
			float_t accelerationDirectionVariance = accelerationField->accelerationDirectionVariance().at(life);

			for(std::uint32_t gridIndex = 0; gridIndex < accelerationStrengthGrid.size(); gridIndex++) {
				fieldData.strengthGrid[gridIndex] = accelerationStrengthVariance * accelerationStrengthGrid[gridIndex];
			}

			for(std::uint32_t gridIndex = 0; gridIndex < accelerationDirectionGrid.size(); gridIndex++) {
				float3_t gridDirectionOffset = math::radians(accelerationDirectionVariance * accelerationDirectionGrid[gridIndex]);
				fieldData.directionMatrixGrid[gridIndex] = matrix3_t(math::yawPitchRollRotationMatrix(gridDirectionOffset.y, gridDirectionOffset.z, gridDirectionOffset.x));
			}

			modifierAccelerationFields.emplace_back(fieldData);
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
		else if(vortexField) {
			modifierVortexFields.emplace_back(*vortexField);
		}
	}
}

void ForceModifier::applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const EffectRuntimeContext& runtimeContext,
	const ParticleType& particleType, id_t particleEmitterId, const AttractionField& attractionField, const SceneGraph& sceneGraph,
	const matrix3_t& globalToSimulationSpaceTransform) const {
	const float_t epsilon = 1.0e-6;

	float_t fieldLife = attractionField.life(runtimeContext);
	Transform fieldTransform = sceneGraph.globalTransform(attractionField.id(), runtimeContext);
	float3_t fieldPosition = fieldTransform.position();
	float3_t fieldSize = fieldTransform.scale() * 0.5;
	float_t fieldStrength = attractionField.strength().at(fieldLife);
	bool fieldInfinite = attractionField.infinite();

	float_t falloffPower = attractionField.falloffPower().at(fieldLife);

	const Curve<float_t>& particleWeightCurve = particleType.weight().resultCurve();

	for(std::uint32_t p = 0; p < particleCount; p++) {
		float3_t particleToCenter = fieldPosition - particles.globalPosition[p];
		float_t distance = math::length(particleToCenter);
		if(distance > fieldSize.x && !fieldInfinite) {
			continue;
		}

		float3_t forceDirection = globalToSimulationSpaceTransform * (particleToCenter / std::max(distance, epsilon));

		particles.force[p] += forceDirection * fieldStrength / std::pow(distance + 1.0, falloffPower) * particleWeightCurve.at(particles.life[p]);
	}
}
void ForceModifier::applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const EffectRuntimeContext& runtimeContext,
	const ParticleType& particleType, id_t particleEmitterId, const AccelerationFieldData& accelerationField, const SceneGraph& sceneGraph,
	const matrix3_t& globalToSimulationSpaceTransform) const {
	float_t fieldLife = accelerationField.forceField.life(runtimeContext);
	Transform fieldTransform = sceneGraph.globalTransform(accelerationField.forceField.id(), runtimeContext);
	matrix3_t fieldRotationMatrix = matrix3_t(math::normalizeTransformationMatrix(fieldTransform.matrix()));
	matrix4_t fieldInverseTransformMatrix = math::inverse(fieldTransform.matrix());
	float_t fieldStrength = accelerationField.forceField.strength().at(fieldLife);
	bool fieldInfinite = accelerationField.forceField.infinite();

	float3_t accelerationDirection = math::radians(accelerationField.forceField.accelerationDirection().at(fieldLife));
	matrix3_t accelerationDirectionMatrix = matrix3_t(math::yawPitchRollRotationMatrix(accelerationDirection.y, accelerationDirection.z, accelerationDirection.x));

	std::int32_t accelerationGridSizeX = accelerationField.forceField.accelerationGridSizeX();
	std::int32_t accelerationGridSizeY = accelerationField.forceField.accelerationGridSizeY();
	std::int32_t accelerationGridSizeZ = accelerationField.forceField.accelerationGridSizeZ();

	const Curve<float_t>& particleWeightCurve = particleType.weight().resultCurve();

	for(std::uint32_t p = 0; p < particleCount; p++) {
		float3_t localParticlePosition = float3_t(fieldInverseTransformMatrix * float4_t(particles.globalPosition[p], 1.0));
		if((localParticlePosition.x < -0.5 || localParticlePosition.x > 0.5 ||
			localParticlePosition.y < -0.5 || localParticlePosition.y > 0.5 ||
			localParticlePosition.z < -0.5 || localParticlePosition.z > 0.5) && !fieldInfinite) {
			continue;
		}

		std::int32_t gridCellX = std::clamp(static_cast<std::int32_t>((localParticlePosition.x + 0.5) * static_cast<float_t>(accelerationGridSizeX)), 0, accelerationGridSizeX - 1);
		std::int32_t gridCellY = std::clamp(static_cast<std::int32_t>((localParticlePosition.y + 0.5) * static_cast<float_t>(accelerationGridSizeY)), 0, accelerationGridSizeY - 1);
		std::int32_t gridCellZ = std::clamp(static_cast<std::int32_t>((localParticlePosition.z + 0.5) * static_cast<float_t>(accelerationGridSizeZ)), 0, accelerationGridSizeZ - 1);
		std::uint32_t gridCellIndex = static_cast<std::uint32_t>(
			gridCellZ * accelerationGridSizeY * accelerationGridSizeX +
			gridCellY * accelerationGridSizeX +
			gridCellX);

		float3_t forceVector = float3_t(globalToSimulationSpaceTransform * fieldRotationMatrix * accelerationField.directionMatrixGrid[gridCellIndex] * accelerationDirectionMatrix * worldUpVector3);

		particles.force[p] += forceVector * (accelerationField.strengthGrid[gridCellIndex] + fieldStrength) * particleWeightCurve.at(particles.life[p]);
	}
}
void ForceModifier::applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const EffectRuntimeContext& runtimeContext,
	const ParticleType& particleType, id_t particleEmitterId, const VectorField& vectorField, const SceneGraph& sceneGraph,
	const matrix3_t& globalToSimulationSpaceTransform) const {
	if(modifierEffectResources == nullptr || modifierEffectResources->vectorFields().count(vectorField.vectorFieldResourceId()) == 0) {
		return;
	}

	float_t fieldLife = vectorField.life(runtimeContext);
	Transform fieldTransform = sceneGraph.globalTransform(vectorField.id(), runtimeContext);
	matrix3_t fieldRotationMatrix = matrix3_t(math::normalizeTransformationMatrix(fieldTransform.matrix()));
	matrix4_t fieldInverseTransformMatrix = math::inverse(fieldTransform.matrix());
	float_t fieldStrength = vectorField.strength().at(fieldLife);
	bool fieldInfinite = vectorField.infinite();

	const VectorFieldResource& vectorFieldResource = modifierEffectResources->vectorFields().at(vectorField.vectorFieldResourceId());
	const Grid3d<float3_t>& vectorFieldGrid = vectorFieldResource.field();
	float3_t vectorFieldGridSize = float3_t(
		static_cast<float_t>(vectorFieldGrid.width()),
		static_cast<float_t>(vectorFieldGrid.height()),
		static_cast<float_t>(vectorFieldGrid.depth()));
	float_t vectorFieldTightness = std::clamp(vectorField.tightness().at(fieldLife), 0.0, 1.0);

	const Curve<float_t>& particleWeightCurve = particleType.weight().resultCurve();

	for(std::uint32_t p = 0; p < particleCount; p++) {
		float3_t localParticlePosition = float3_t(fieldInverseTransformMatrix * float4_t(particles.globalPosition[p], 1.0));
		if((localParticlePosition.x < -0.5 || localParticlePosition.x > 0.5 ||
			localParticlePosition.y < -0.5 || localParticlePosition.y > 0.5 ||
			localParticlePosition.z < -0.5 || localParticlePosition.z > 0.5) && !fieldInfinite) {
			continue;
		}

		float3_t forceVector = float3_t(0.0);
		float3_t samplePosition = localParticlePosition + float3_t(0.5);
		float3_t gridSamplePosition = samplePosition * vectorFieldGridSize;

		if(modifierEffect3d) {
			switch(vectorField.vectorFieldFilter()) {
				case VectorField::Filter::none: {
					forceVector = vectorFieldGrid.value(
						static_cast<std::int32_t>(gridSamplePosition.x),
						static_cast<std::int32_t>(gridSamplePosition.y),
						static_cast<std::int32_t>(gridSamplePosition.z),
						float3_t(0.0));

					break;
				}

				case VectorField::Filter::linear: {
					float_t fractX = math::fract(gridSamplePosition.x);
					float_t fractY = math::fract(gridSamplePosition.y);
					float_t fractZ = math::fract(gridSamplePosition.z);
					std::int32_t nextOffsetX = fractX > 0.5 ? +1 : -1;
					std::int32_t nextOffsetY = fractY > 0.5 ? +1 : -1;
					std::int32_t nextOffsetZ = fractZ > 0.5 ? +1 : -1;

					float3_t sample0 = vectorFieldGrid.value(
						static_cast<std::int32_t>(gridSamplePosition.x),
						static_cast<std::int32_t>(gridSamplePosition.y),
						static_cast<std::int32_t>(gridSamplePosition.z),
						float3_t(0.0));
					float3_t sample1 = vectorFieldGrid.value(
						static_cast<std::int32_t>(gridSamplePosition.x) + nextOffsetX,
						static_cast<std::int32_t>(gridSamplePosition.y),
						static_cast<std::int32_t>(gridSamplePosition.z),
						float3_t(0.0));
					float3_t sample2 = vectorFieldGrid.value(
						static_cast<std::int32_t>(gridSamplePosition.x),
						static_cast<std::int32_t>(gridSamplePosition.y) + nextOffsetY,
						static_cast<std::int32_t>(gridSamplePosition.z),
						float3_t(0.0));
					float3_t sample3 = vectorFieldGrid.value(
						static_cast<std::int32_t>(gridSamplePosition.x) + nextOffsetX,
						static_cast<std::int32_t>(gridSamplePosition.y) + nextOffsetY,
						static_cast<std::int32_t>(gridSamplePosition.z),
						float3_t(0.0));
					float3_t sample4 = vectorFieldGrid.value(
						static_cast<std::int32_t>(gridSamplePosition.x),
						static_cast<std::int32_t>(gridSamplePosition.y),
						static_cast<std::int32_t>(gridSamplePosition.z) + nextOffsetZ,
						float3_t(0.0));
					float3_t sample5 = vectorFieldGrid.value(
						static_cast<std::int32_t>(gridSamplePosition.x) + nextOffsetX,
						static_cast<std::int32_t>(gridSamplePosition.y),
						static_cast<std::int32_t>(gridSamplePosition.z) + nextOffsetZ,
						float3_t(0.0));
					float3_t sample6 = vectorFieldGrid.value(
						static_cast<std::int32_t>(gridSamplePosition.x),
						static_cast<std::int32_t>(gridSamplePosition.y) + nextOffsetY,
						static_cast<std::int32_t>(gridSamplePosition.z) + nextOffsetZ,
						float3_t(0.0));
					float3_t sample7 = vectorFieldGrid.value(
						static_cast<std::int32_t>(gridSamplePosition.x) + nextOffsetX,
						static_cast<std::int32_t>(gridSamplePosition.y) + nextOffsetY,
						static_cast<std::int32_t>(gridSamplePosition.z) + nextOffsetZ,
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
					forceVector = vectorFieldGrid.value(
						static_cast<std::int32_t>(gridSamplePosition.x),
						static_cast<std::int32_t>(gridSamplePosition.y), 0,
						float3_t(0.0));

					break;
				}

				case VectorField::Filter::linear: {
					float_t fractX = math::fract(gridSamplePosition.x);
					float_t fractY = math::fract(gridSamplePosition.y);
					std::int32_t nextOffsetX = fractX > 0.5 ? +1 : -1;
					std::int32_t nextOffsetY = fractY > 0.5 ? +1 : -1;

					float3_t sample0 = vectorFieldGrid.value(
						static_cast<std::int32_t>(gridSamplePosition.x),
						static_cast<std::int32_t>(gridSamplePosition.y), 0,
						float3_t(0.0));
					float3_t sample1 = vectorFieldGrid.value(
						static_cast<std::int32_t>(gridSamplePosition.x) + nextOffsetX,
						static_cast<std::int32_t>(gridSamplePosition.y), 0,
						float3_t(0.0));
					float3_t sample2 = vectorFieldGrid.value(
						static_cast<std::int32_t>(gridSamplePosition.x),
						static_cast<std::int32_t>(gridSamplePosition.y) + nextOffsetY, 0,
						float3_t(0.0));
					float3_t sample3 = vectorFieldGrid.value(
						static_cast<std::int32_t>(gridSamplePosition.x) + nextOffsetX,
						static_cast<std::int32_t>(gridSamplePosition.y) + nextOffsetY, 0,
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

		forceVector = globalToSimulationSpaceTransform * fieldRotationMatrix * forceVector;
		forceVector *= fieldStrength * particleWeightCurve.at(particles.life[p]);

		particles.force[p] += forceVector * (1.0 - vectorFieldTightness);
		particles.velocity[p] *= 1.0 - vectorFieldTightness;
		particles.velocity[p] += forceVector * vectorFieldTightness;
	}
}
void ForceModifier::applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const EffectRuntimeContext& runtimeContext,
	const ParticleType& particleType, id_t particleEmitterId, const NoiseField& noiseField, const SceneGraph& sceneGraph,
	const matrix3_t& globalToSimulationSpaceTransform) const {
	float_t fieldLife = noiseField.life(runtimeContext);
	Transform fieldTransform = sceneGraph.globalTransform(noiseField.id(), runtimeContext);
	matrix3_t fieldRotationMatrix = matrix3_t(math::normalizeTransformationMatrix(fieldTransform.matrix()));
	matrix4_t fieldInverseTransformMatrix = math::inverse(fieldTransform.matrix());
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
		float3_t localParticlePosition = float3_t(fieldInverseTransformMatrix * float4_t(particles.globalPosition[p], 1.0));
		if((localParticlePosition.x < -0.5 || localParticlePosition.x > 0.5 ||
			localParticlePosition.y < -0.5 || localParticlePosition.y > 0.5 ||
			localParticlePosition.z < -0.5 || localParticlePosition.z > 0.5) && !fieldInfinite) {
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

		forceVector = globalToSimulationSpaceTransform * fieldRotationMatrix * forceVector;

		particles.force[p] += forceVector * fieldStrength * particleWeightCurve.at(particles.life[p]);
	}
}
void ForceModifier::applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const EffectRuntimeContext& runtimeContext,
	const ParticleType& particleType, id_t particleEmitterId, const DragField& dragField, const SceneGraph& sceneGraph,
	const matrix3_t& globalToSimulationSpaceTransform) const {
	const float_t epsilon = 1.0e-6;

	float_t fieldLife = dragField.life(runtimeContext);
	Transform fieldTransform = sceneGraph.globalTransform(dragField.id(), runtimeContext);
	matrix4_t fieldInverseTransformMatrix = math::inverse(fieldTransform.matrix());
	float_t fieldStrength = dragField.strength().at(fieldLife);
	bool fieldInfinite = dragField.infinite();

	float_t dragVelocityInfluence = dragField.velocityInfluence().value();
	float_t dragSizeInfluence = dragField.sizeInfluence().value();

	const Curve<float_t>& particleWeightCurve = particleType.weight().resultCurve();
	const Curve<float_t>& particlePhysicalSizeCurve = particleType.physicalSize().resultCurve();

	for(std::uint32_t p = 0; p < particleCount; p++) {
		float3_t localParticlePosition = float3_t(fieldInverseTransformMatrix * float4_t(particles.globalPosition[p], 1.0));
		if((localParticlePosition.x < -0.5 || localParticlePosition.x > 0.5 ||
			localParticlePosition.y < -0.5 || localParticlePosition.y > 0.5 ||
			localParticlePosition.z < -0.5 || localParticlePosition.z > 0.5) && !fieldInfinite) {
			continue;
		}

		float_t particleSpeed = std::max(math::length(particles.velocity[p]), epsilon);
		float3_t particleDirection = particles.velocity[p] / particleSpeed;

		float3_t particleSize = particles.size[p] * particlePhysicalSizeCurve.at(particles.life[p]);
		float_t particleArea = std::max(particleSize.x, std::max(particleSize.y, particleSize.z));

		float3_t forceVector = -particleDirection *
			(1.0 + (particleSpeed * particleSpeed - 1.0) * dragVelocityInfluence) *
			(1.0 + (particleArea - 1.0) * dragSizeInfluence);

		particles.force[p] += forceVector * fieldStrength * particleWeightCurve.at(particles.life[p]);
	}
}
void ForceModifier::applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const EffectRuntimeContext& runtimeContext,
	const ParticleType& particleType, id_t particleEmitterId, const VortexField& vortexField, const SceneGraph& sceneGraph,
	const matrix3_t& globalToSimulationSpaceTransform) const {
	const float_t epsilon = 1.0e-6;
	const float3_t axis = float3_t(0.0, 0.0, 1.0);
	const float3_t rightAxis = float3_t(1.0, 0.0, 0.0);
	const float3_t upAxis = float3_t(0.0, 1.0, 0.0);

	float_t fieldLife = vortexField.life(runtimeContext);
	Transform fieldTransform = sceneGraph.globalTransform(vortexField.id(), runtimeContext);
	matrix3_t fieldRotationMatrix = matrix3_t(math::normalizeTransformationMatrix(fieldTransform.matrix()));
	matrix4_t fieldInverseTransformMatrix = math::inverse(fieldTransform.matrix());
	float_t fieldStrength = vortexField.strength().at(fieldLife);
	bool fieldInfinite = vortexField.infinite();

	float_t tangentialStrength = vortexField.tangentialStrength().at(fieldLife);
	float_t radialStrength = vortexField.radialStrength().at(fieldLife);

	const Curve<float_t>& particleWeightCurve = particleType.weight().resultCurve();

	for(std::uint32_t p = 0; p < particleCount; p++) {
		float3_t localParticlePosition = float3_t(fieldInverseTransformMatrix * float4_t(particles.globalPosition[p], 1.0));

		float_t axisDistance = math::dot(localParticlePosition, axis);

		float_t crossSectionDistanceX = math::dot(localParticlePosition, rightAxis);
		float_t crossSectionDistanceY = math::dot(localParticlePosition, upAxis);
		float_t crossSectionDistance =
			crossSectionDistanceX * crossSectionDistanceX / 0.25 +
			crossSectionDistanceY * crossSectionDistanceY / 0.25;

		if((axisDistance < -0.5 || axisDistance > 0.5 || crossSectionDistance > 1.0) && !fieldInfinite) {
			continue;
		}

		float3_t axisToParticle = localParticlePosition - axis * axisDistance;
		float_t distanceToAxis = math::length(axisToParticle);

		float3_t radialDirection = axisToParticle / std::max(distanceToAxis, epsilon);
		float3_t tangentDirection = math::safeNormalize(math::cross(axis, radialDirection));

		float3_t forceVector = globalToSimulationSpaceTransform * fieldRotationMatrix * (tangentDirection * tangentialStrength - radialDirection * radialStrength);

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
