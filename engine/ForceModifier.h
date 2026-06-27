#pragma once

#include "ParticleModifier.h"
#include "../effect/AttractionField.h"
#include "../effect/AccelerationField.h"
#include "../effect/VectorField.h"
#include "../effect/NoiseField.h"
#include "../effect/DragField.h"
#include "../effect/ResourceCollection.h"
#include "../effect/VectorFieldResource.h"
#include "../types/Types.h"
#include <vector>

namespace pixelpart {
class ForceModifier : public ParticleModifier {
public:
	ForceModifier() = default;

	virtual void apply(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
		const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const override;

	virtual void reset(const Effect* effect, EffectRuntimeContext runtimeContext) override;

private:
	void applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const EffectRuntimeContext& runtimeContext,
		const ParticleType& particleType, const AttractionField& attractionField, const SceneGraph& sceneGraph) const;
	void applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const EffectRuntimeContext& runtimeContext,
		const ParticleType& particleType, const AccelerationField& accelerationField, const SceneGraph& sceneGraph) const;
	void applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const EffectRuntimeContext& runtimeContext,
		const ParticleType& particleType, const VectorField& vectorField, const SceneGraph& sceneGraph) const;
	void applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const EffectRuntimeContext& runtimeContext,
		const ParticleType& particleType, const NoiseField& noiseField, const SceneGraph& sceneGraph) const;
	void applyForce(ParticleCollection::WritePtr particles, std::uint32_t particleCount, const EffectRuntimeContext& runtimeContext,
		const ParticleType& particleType, const DragField& dragField, const SceneGraph& sceneGraph) const;

	float3_t computeStaticCurlNoise2d(const float2_t& samplePosition, std::uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const;
	float3_t computeStaticCurlNoise3d(const float3_t& samplePosition, std::uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const;
	float3_t computeAnimatedCurlNoise2d(const float2_t& samplePosition, float_t animationTime, std::uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const;
	float3_t computeAnimatedCurlNoise3d(const float3_t& samplePosition, float_t animationTime, std::uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const;

	const ResourceCollection* modifierEffectResources = nullptr;
	bool modifierEffect3d = false;

	std::vector<AttractionField> modifierAttractionFields;
	std::vector<AccelerationField> modifierAccelerationFields;
	std::vector<VectorField> modifierVectorFields;
	std::vector<NoiseField> modifierNoiseFields;
	std::vector<DragField> modifierDragFields;
};
}
