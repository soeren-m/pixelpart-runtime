#pragma once

#include "../effect/Effect.h"
#include "ParticleData.h"
#include <bitset>

namespace pixelpart {
class ForceSolver {
public:
	ForceSolver();

	void solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t numParticles, float_t t, float_t dt) const;
	void solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t numParticles, float_t t, float_t dt, const ForceField& forceField) const;

	void update(const Effect* effect);

private:
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

	const EffectResourceSet* effectResources = nullptr;
	bool is3d = false;

	std::vector<ForceField> forceFields;
	std::vector<std::bitset<256>> forceFieldExclusionSets;
};
}