#pragma once

#include "../common/Types.h"
#include "../common/Math.h"
#include "../effect/Effect.h"
#include "../effect/ParticleEmitter.h"
#include "ParticleCollection.h"
#include "Random.h"
#include <random>
#include <vector>

namespace pixelpart {
class ParticleGenerator {
public:
	ParticleGenerator(const Effect& fx, std::vector<ParticleCollection>& particleColl);

	uint32_t generate(uint32_t count, uint32_t parentParticle, uint32_t particleTypeIndex, uint32_t parentParticleTypeIndex, uint32_t particleEmitterIndex, float_t dt, float_t t);

	void prepare();
	void reset();

	void applySeed(uint32_t seed);

private:
	static float_t sampleGrid1D(uint32_t gridIndex, uint32_t gridSize, float_t min, float_t max);
	static float_t sampleGrid2D(uint32_t gridIndex, uint32_t gridSize1, uint32_t gridSize2, float_t min, float_t max);
	static float_t sampleGrid3D(uint32_t gridIndex, uint32_t gridSize1, uint32_t gridSize2, uint32_t gridSize3, float_t min, float_t max);

	static float3_t rotate2d(const float3_t& v, float_t a);
	static float3_t rotate3d(const float3_t& v, const float3_t& a);

	float3_t emitOnSegment(float_t length,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		uint32_t gridSize, uint32_t& gridIndex);

	float3_t emitInEllipse(const float2_t& size,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		uint32_t gridSizeX, uint32_t gridSizeY, uint32_t& gridIndex);

	float3_t emitInRectangle(const float2_t& size,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		uint32_t gridSizeX, uint32_t gridSizeY, uint32_t& gridIndex);

	float3_t emitOnPath(const Curve<float3_t>& path,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		uint32_t gridSize, uint32_t& gridIndex);

	float3_t emitInEllipsoid(const float3_t& size,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		uint32_t gridSizeX, uint32_t gridSizeY, uint32_t gridSizeZ, uint32_t& gridIndex);

	float3_t emitInCuboid(const float3_t& size,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		uint32_t gridSizeX, uint32_t gridSizeY, uint32_t gridSizeZ, uint32_t& gridIndex);

	float3_t emitInCylinder(const float3_t& size,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		uint32_t gridSizeX, uint32_t gridSizeY, uint32_t gridSizeZ, uint32_t& gridIndex);

	const Effect& effect;
	std::vector<ParticleCollection>& particleCollections;

	uint32_t nextParticleId = 0u;
	std::vector<uint32_t> particleEmitterGridIndices;
	std::mt19937 rng;
};
}