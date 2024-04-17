#pragma once

#include "ParticleSolver.h"
#include <bitset>

namespace pixelpart {
class ForceSolver : public ParticleSolver {
public:
	ForceSolver();

	virtual void solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		ParticleWritePtr particles, uint32_t numParticles, float_t t, float_t dt) const override;

	virtual void refresh(const Effect& effect) override;

private:
	void solve(const ParticleType& particleType, ParticleWritePtr particles, uint32_t numParticles, float_t t, float_t dt, const ForceField& forceField) const;

	vec3_t sampleAttractionField(const ForceField::AttractionField& attractionField,
		const vec3_t& position, float_t size,
		const vec3_t& particlePosition) const;
	vec3_t sampleAccelerationField(const ForceField::AccelerationField& accelerationField,
		const vec3_t& position, const vec3_t& size, const mat4_t& orientationMatrix, const mat4_t& directionMatrix,
		const vec3_t& particlePosition) const;
	vec3_t sampleVectorField(const ForceField::VectorField& vectorField, const VectorFieldResource& resource,
		const vec3_t& position, const vec3_t& size, const mat4_t& directionMatrix, const mat4_t& orientationMatrix,
		const vec3_t& particlePosition, bool& inside) const;
	vec3_t sampleNoiseField(const ForceField::NoiseField& noiseField,
		const vec3_t& position, const vec3_t& size, const mat4_t& orientationMatrix,
		const vec3_t& particlePosition, float_t life, float_t t) const;
	vec3_t sampleDragField(const ForceField::DragField& dragField,
		const vec3_t& position, const vec3_t& size, const mat4_t& orientationMatrix,
		const vec3_t& particlePosition, const vec3_t& particleVelocity, const vec3_t& particleSize) const;

	vec3_t computeStaticCurlNoise2d(const vec2_t& samplePosition, uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const;
	vec3_t computeStaticCurlNoise3d(const vec3_t& samplePosition, uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const;
	vec3_t computeAnimatedCurlNoise2d(const vec2_t& samplePosition, float_t animationTime, uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const;
	vec3_t computeAnimatedCurlNoise3d(const vec3_t& samplePosition, float_t animationTime, uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity) const;

	const ResourceCollection* effectResources = nullptr;
	bool is3d = false;

	std::vector<ForceField> forceFields;
	std::vector<std::bitset<256>> forceFieldExclusionSets;
};
}