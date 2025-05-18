#pragma once

#include "../common/Types.h"
#include "../common/Math.h"
#include "../effect/Effect.h"
#include "../effect/ParticleEmitter.h"
#include "../effect/RuntimeContext.h"
#include "ParticleCollection.h"
#include "ParticleRuntimeInstance.h"
#include "ParticleRuntimeInstanceCollection.h"
#include "Random.h"
#include <random>

namespace pixelpart {
class ParticleGenerator {
public:
	ParticleGenerator(const Effect& fx, ParticleRuntimeInstanceCollection& particleRuntimeInsts);

	void generate(const RuntimeContext& runtimeContext);
	void generate(id_t particleEmitterId, id_t particleTypeId, std::uint32_t count, float_t time);

	void reset();

	void seed(std::uint32_t seed);

private:
	std::uint32_t generate(ParticleRuntimeInstance& runtimeInstance, ParticleRuntimeInstance* parentRuntimeInstance, std::uint32_t count,
		std::uint32_t parentParticle, const RuntimeContext& runtimeContext, bool useTriggers);

	static float_t sampleGrid1D(std::uint32_t gridIndex, std::uint32_t gridSize, float_t min, float_t max);
	static float_t sampleGrid2D(std::uint32_t gridIndex, std::uint32_t gridSize1, std::uint32_t gridSize2, float_t min, float_t max);
	static float_t sampleGrid3D(std::uint32_t gridIndex, std::uint32_t gridSize1, std::uint32_t gridSize2, std::uint32_t gridSize3, float_t min, float_t max);

	static float3_t rotate2d(const float3_t& v, float_t a);
	static float3_t rotate3d(const float3_t& v, const float3_t& a);

	float3_t emitOnSegment(float_t length,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		std::uint32_t gridSize, std::uint32_t& gridIndex);

	float3_t emitInEllipse(const float2_t& size,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t& gridIndex);

	float3_t emitInRectangle(const float2_t& size,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t& gridIndex);

	float3_t emitOnPath(const float3_t& size,
		const Curve<float3_t>& path,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		std::uint32_t gridSize, std::uint32_t& gridIndex);

	float3_t emitInEllipsoid(const float3_t& size,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t gridSizeZ, std::uint32_t& gridIndex);

	float3_t emitInCuboid(const float3_t& size,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t gridSizeZ, std::uint32_t& gridIndex);

	float3_t emitInCylinder(const float3_t& size,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t gridSizeZ, std::uint32_t& gridIndex);

	const Effect& effect;
	ParticleRuntimeInstanceCollection& particleRuntimeInstances;

	std::uint32_t nextParticleId = 0;
	std::mt19937 rng;
};
}