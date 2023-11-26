#pragma once

#include "../effect/Effect.h"
#include "ParticleData.h"
#include <bitset>

namespace pixelpart {
class ForceSolver {
public:
	ForceSolver();

	void solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t numParticles, floatd t, floatd dt) const;
	void solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t numParticles, floatd t, floatd dt, const ForceField& forceField) const;

	void update(const Effect* effect);

private:
	vec3d sampleAttractionField(const ForceField::AttractionField& attractionField,
		const vec3d& position, floatd size,
		const vec3d& particlePosition) const;
	vec3d sampleAccelerationField(const ForceField::AccelerationField& accelerationField,
		const vec3d& position, const vec3d& size, const mat4d& orientationMatrix, const mat4d& directionMatrix,
		const vec3d& particlePosition) const;
	vec3d sampleVectorField(const ForceField::VectorField& vectorField, const VectorFieldResource& resource,
		const vec3d& position, const vec3d& size, const mat4d& directionMatrix, const mat4d& orientationMatrix,
		const vec3d& particlePosition, bool& inside) const;
	vec3d sampleNoiseField(const ForceField::NoiseField& noiseField,
		const vec3d& position, const vec3d& size, const mat4d& orientationMatrix,
		const vec3d& particlePosition, floatd life, floatd t) const;
	vec3d sampleDragField(const ForceField::DragField& dragField,
		const vec3d& position, const vec3d& size, const mat4d& orientationMatrix,
		const vec3d& particlePosition, const vec3d& particleVelocity, const vec3d& particleSize) const;

	vec3d computeStaticCurlNoise2d(const vec2d& samplePosition, uint32_t octaves, floatd frequency, floatd persistence, floatd lacunarity) const;
	vec3d computeStaticCurlNoise3d(const vec3d& samplePosition, uint32_t octaves, floatd frequency, floatd persistence, floatd lacunarity) const;
	vec3d computeAnimatedCurlNoise2d(const vec2d& samplePosition, floatd animationTime, uint32_t octaves, floatd frequency, floatd persistence, floatd lacunarity) const;
	vec3d computeAnimatedCurlNoise3d(const vec3d& samplePosition, floatd animationTime, uint32_t octaves, floatd frequency, floatd persistence, floatd lacunarity) const;

	const EffectResourceSet* effectResources = nullptr;
	bool is3d = false;

	std::vector<ForceField> forceFields;
	std::vector<std::bitset<256>> forceFieldExclusionSets;
};
}