#pragma once

#include "../common/Types.h"
#include "../common/Math.h"
#include "../effect/Effect.h"
#include "../effect/ParticleEmitter.h"
#include "ParticleCollection.h"
#include "ParticleRuntimeInstance.h"
#include "ParticleRuntimeInstanceCollection.h"
#include "Random.h"
#include <random>

namespace pixelpart {
class ParticleGenerator {
public:
	ParticleGenerator(const Effect& fx, ParticleRuntimeInstanceCollection& particleRuntimeInsts);

	void generate(float_t time, float_t dt);
	void generate(id_t particleEmitterId, id_t particleTypeId, uint32_t count, float_t time);

	void reset();

	void seed(uint32_t seed);

private:
	uint32_t generate(ParticleRuntimeInstance& runtimeInstance, uint32_t count, float_t time, float_t particleLifeFraction);
	uint32_t generate(ParticleRuntimeInstance& runtimeInstance, ParticleRuntimeInstance* parentRuntimeInstance, uint32_t count, uint32_t parentParticle, float_t time, float_t particleLifeFraction);

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
	ParticleRuntimeInstanceCollection& particleRuntimeInstances;

	uint32_t nextParticleId = 0u;
	std::mt19937 rng;
};
}