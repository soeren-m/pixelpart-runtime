#pragma once

#include "ParticleSolver.h"
#include "../common/Math.h"
#include "../effect/ForceField.h"
#include "../effect/ResourceCollection.h"
#include "../effect/VectorFieldResource.h"
#include <bitset>
#include <vector>

namespace pixelpart {
class ForceSolver : public ParticleSolver {
public:
	ForceSolver();

	virtual void solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		ParticleCollection::WritePtr particles, uint32_t particleCount, float_t t, float_t dt) const override;

	virtual void prepare(const Effect& effect) override;

private:
	void solve(const ParticleType& particleType, ParticleCollection::WritePtr particles, uint32_t particleCount, float_t t, float_t dt, const ForceField& forceField) const;

	float3_t sampleAttractionField(const ForceField& forceField,
		const float3_t& position, float_t size,
		const float3_t& particlePosition) const;
	float3_t sampleAccelerationField(const ForceField& forceField,
		const float3_t& position, const float3_t& size, const mat4_t& orientationMatrix, const mat4_t& directionMatrix,
		const float3_t& particlePosition) const;
	float3_t sampleVectorField(const ForceField& forceField, const VectorFieldResource& resource,
		const float3_t& position, const float3_t& size, const mat4_t& directionMatrix, const mat4_t& orientationMatrix,
		const float3_t& particlePosition, bool& inside) const;
	float3_t sampleNoiseField(const ForceField& forceField,
		const float3_t& position, const float3_t& size, const mat4_t& orientationMatrix,
		const float3_t& particlePosition, float_t life, float_t t) const;
	float3_t sampleDragField(const ForceField& forceField,
		const float3_t& position, const float3_t& size, const mat4_t& orientationMatrix,
		const float3_t& particlePosition, const float3_t& particleVelocity, const float3_t& particleSize) const;

	float3_t computeStaticCurlNoise2d(const float2_t& samplePosition, uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const;
	float3_t computeStaticCurlNoise3d(const float3_t& samplePosition, uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const;
	float3_t computeAnimatedCurlNoise2d(const float2_t& samplePosition, float_t animationTime, uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const;
	float3_t computeAnimatedCurlNoise3d(const float3_t& samplePosition, float_t animationTime, uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const;

	const ResourceCollection* effectResources = nullptr;
	bool is3d = false;

	std::vector<ForceField> forceFields;
	std::vector<std::bitset<256>> forceFieldExclusionSets;
};
}