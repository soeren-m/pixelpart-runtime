#pragma once

#include "../effect/Effect.h"
#include "ParticleContainer.h"
#include "Random.h"

namespace pixelpart {
class ParticleGenerator {
public:
	ParticleGenerator(const Effect& fx, std::vector<ParticleContainer>& state);

	uint32_t generate(uint32_t count, uint32_t parentParticle, uint32_t particleTypeIndex, uint32_t parentParticleTypeIndex, uint32_t particleEmitterIndex, float_t dt, float_t t);

	void prepare();
	void reset();

	void applySeed(uint32_t seed);

private:
	static float_t sampleGrid1D(uint32_t gridIndex, uint32_t gridSize, float_t min, float_t max);
	static float_t sampleGrid2D(uint32_t gridIndex, uint32_t gridSize1, uint32_t gridSize2, float_t min, float_t max);
	static float_t sampleGrid3D(uint32_t gridIndex, uint32_t gridSize1, uint32_t gridSize2, uint32_t gridSize3, float_t min, float_t max);

	static vec3_t rotate2d(const vec3_t& v, float_t a);
	static vec3_t rotate3d(const vec3_t& v, const vec3_t& a);

	vec3_t emitOnSegment(float_t length,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		uint32_t gridSize, uint32_t& gridIndex);

	vec3_t emitInEllipse(const vec2_t& size,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		uint32_t gridSizeX, uint32_t gridSizeY, uint32_t& gridIndex);

	vec3_t emitInRectangle(const vec2_t& size,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		uint32_t gridSizeX, uint32_t gridSizeY, uint32_t& gridIndex);

	vec3_t emitOnPath(const Curve<vec3_t>& path,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		uint32_t gridSize, uint32_t& gridIndex);

	vec3_t emitInEllipsoid(const vec3_t& size,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		uint32_t gridSizeX, uint32_t gridSizeY, uint32_t gridSizeZ, uint32_t& gridIndex);

	vec3_t emitInCuboid(const vec3_t& size,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		uint32_t gridSizeX, uint32_t gridSizeY, uint32_t gridSizeZ, uint32_t& gridIndex);

	vec3_t emitInCylinder(const vec3_t& size,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		uint32_t gridSizeX, uint32_t gridSizeY, uint32_t gridSizeZ, uint32_t& gridIndex);

	const Effect& effect;
	std::vector<ParticleContainer>& particleState;

	uint32_t nextParticleId = 0u;
	std::vector<uint32_t> particleEmitterGridIndices;
	std::mt19937 rng;
};
}