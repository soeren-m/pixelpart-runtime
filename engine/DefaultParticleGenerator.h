#pragma once

#include "ParticleGenerator.h"
#include "../effect/Curve.h"
#include "../effect/Transform.h"
#include "../effect/ParticleEmitter.h"
#include "../effect/ParticleType.h"
#include "../effect/ParticleEmissionPair.h"
#include "../types/Types.h"
#include <random>

namespace pixelpart {
class DefaultParticleGenerator : public ParticleGenerator {
public:
	DefaultParticleGenerator() = default;

	virtual void generate(EffectRuntimeState& state,
		const Effect* effect, EffectRuntimeContext runtimeContext) const override;
	virtual void clear(EffectRuntimeState& state) const override;

	virtual void generate(EffectRuntimeState& state, std::uint32_t count,
		const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const override;

private:
	static std::uint32_t initializeParticles(std::uint32_t count, float_t lifetimeFactor,
		EffectRuntimeState& state, ParticleEmissionState& emissionState,
		ParticleCollection& particleCollection, const ParticleCollection* parentParticleCollection, std::uint32_t parentParticle,
		const Effect* effect, ParticleEmissionPair emissionPair, EffectRuntimeContext runtimeContext,
		bool useTriggers);

	static float3_t emitOnSegment(float_t length,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		std::uint32_t gridSize, std::uint32_t& gridIndex,
		std::mt19937& rng);

	static float3_t emitInEllipse(const float2_t& size,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t& gridIndex,
		std::mt19937& rng);

	static float3_t emitInRectangle(const float2_t& size,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t& gridIndex,
		std::mt19937& rng);

	static float3_t emitOnPath(const float3_t& size,
		const Curve<float3_t>& path,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		std::uint32_t gridSize, std::uint32_t& gridIndex,
		std::mt19937& rng);

	static float3_t emitInEllipsoid(const float3_t& size,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t gridSizeZ, std::uint32_t& gridIndex,
		std::mt19937& rng);

	static float3_t emitInCuboid(const float3_t& size,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t gridSizeZ, std::uint32_t& gridIndex,
		std::mt19937& rng);

	static float3_t emitInCylinder(const float3_t& size,
		ParticleEmitter::Distribution distribution,
		ParticleEmitter::GridOrder gridOrder,
		std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t gridSizeZ, std::uint32_t& gridIndex,
		std::mt19937& rng);

	static float_t sampleGrid1d(std::uint32_t gridIndex, std::uint32_t gridSize, float_t min, float_t max);
	static float_t sampleGrid2d(std::uint32_t gridIndex, std::uint32_t gridSize1, std::uint32_t gridSize2, float_t min, float_t max);
	static float_t sampleGrid3d(std::uint32_t gridIndex, std::uint32_t gridSize1, std::uint32_t gridSize2, std::uint32_t gridSize3, float_t min, float_t max);
};
}
