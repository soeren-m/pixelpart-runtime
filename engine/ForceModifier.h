#pragma once

#include "ParticleModifier.h"
#include "../common/Math.h"
#include "../effect/AttractionField.h"
#include "../effect/AccelerationField.h"
#include "../effect/VectorField.h"
#include "../effect/NoiseField.h"
#include "../effect/DragField.h"
#include "../effect/ResourceCollection.h"
#include "../effect/VectorFieldResource.h"
#include <vector>

namespace pixelpart {
class ForceModifier : public ParticleModifier {
public:
	ForceModifier() = default;

	virtual void run(const SceneGraph& sceneGraph, const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		ParticleCollection::WritePtr particles, std::uint32_t particleCount, const RuntimeContext& runtimeContext) const override;

	virtual void prepare(const Effect& effect, const RuntimeContext& runtimeContext) override;

private:
	void applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const RuntimeContext& runtimeContext,
		const ParticleType& particleType, const AttractionField& attractionField, const SceneGraph& sceneGraph) const;
	void applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const RuntimeContext& runtimeContext,
		const ParticleType& particleType, const AccelerationField& accelerationField, const SceneGraph& sceneGraph) const;
	void applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const RuntimeContext& runtimeContext,
		const ParticleType& particleType, const VectorField& vectorField, const SceneGraph& sceneGraph) const;
	void applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const RuntimeContext& runtimeContext,
		const ParticleType& particleType, const NoiseField& noiseField, const SceneGraph& sceneGraph) const;
	void applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const RuntimeContext& runtimeContext,
		const ParticleType& particleType, const DragField& dragField, const SceneGraph& sceneGraph) const;

	float3_t sampleAttractionField(const AttractionField& attractionField,
		const float3_t& position, float_t size,
		const float3_t& particlePosition) const;
	float3_t sampleAccelerationField(const AccelerationField& accelerationField,
		const float3_t& position, const float3_t& size, const mat4_t& rotationMatrix, const mat4_t& directionMatrix,
		const float3_t& particlePosition) const;
	float3_t sampleVectorField(const VectorField& vectorField, const VectorFieldResource& resource,
		const float3_t& position, const float3_t& size, const mat4_t& rotationMatrix, const mat4_t& directionMatrix,
		const float3_t& particlePosition, bool& inside) const;
	float3_t sampleNoiseField(const NoiseField& noiseField,
		const float3_t& position, const float3_t& size, const mat4_t& rotationMatrix,
		const float3_t& particlePosition, float_t life, float_t t) const;
	float3_t sampleDragField(const DragField& dragField,
		const float3_t& position, const float3_t& size, const mat4_t& rotationMatrix,
		const float3_t& particlePosition, const float3_t& particleVelocity, const float3_t& particleSize) const;

	float3_t computeStaticCurlNoise2d(const float2_t& samplePosition, std::uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const;
	float3_t computeStaticCurlNoise3d(const float3_t& samplePosition, std::uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const;
	float3_t computeAnimatedCurlNoise2d(const float2_t& samplePosition, float_t animationTime, std::uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const;
	float3_t computeAnimatedCurlNoise3d(const float3_t& samplePosition, float_t animationTime, std::uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const;

	const ResourceCollection* effectResources = nullptr;
	bool is3d = false;

	std::vector<AttractionField> attractionFields;
	std::vector<AccelerationField> accelerationFields;
	std::vector<VectorField> vectorFields;
	std::vector<NoiseField> noiseFields;
	std::vector<DragField> dragFields;
};
}