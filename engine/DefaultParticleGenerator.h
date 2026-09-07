#pragma once

#include "ParticleGenerator.h"
#include "../effect/Polyline.h"
#include "../effect/Transform.h"
#include "../effect/ParticleEmitter.h"
#include "../effect/ParticleType.h"
#include "../effect/ParticleEmissionPair.h"
#include "../math/Pcg32.h"
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
	struct ParticleEmitterEmissionData {
		Transform globalTransform;
		Transform globalPrevTransform;
		float3_t globalPosition;
		float3_t globalRotation;
		float3_t globalScale;
		matrix3_t globalRotationMatrix;
		ParticleEmitter::Shape shape;
		Polyline path;
		ParticleEmitter::Distribution distribution;
		ParticleEmitter::GridOrder gridOrder;
		std::uint32_t gridSize[3];
		ParticleEmitter::EmissionMode emissionMode;
		ParticleEmitter::DirectionMode directionMode;
		float3_t direction;
		float_t spread;
		float3_t velocity;

		ParticleEmitterEmissionData(const Effect* effect, id_t particleEmitterId, EffectRuntimeContext runtimeContext, bool useTriggers);
	};

	struct ParticleTypeEmissionData {
		bool localCoords;
		float_t lifespan;
		float_t lifespanVariance;
		float_t initialVelocity;
		float_t inheritedVelocity;
		float_t velocityVariance;
		float3_t initialRotation;
		float3_t rotationVariance;
		float3_t angularVelocityVariance;
		float_t initialSize;
		float_t sizeVariance;
		float3_t size;
		float_t initialOpacity;
		float4_t colorVariance;
		float_t opacityVariance;
		float4_t color;

		ParticleTypeEmissionData(const Effect* effect, ParticleEmissionPair emissionPair, EffectRuntimeContext runtimeContext, bool useTriggers);
	};

	static std::uint32_t initializeParticles(std::uint32_t count, float_t lifetimeFactor,
		EffectRuntimeState& state, ParticleEmissionState& emissionState,
		ParticleCollection& particleCollection, const ParticleCollection* parentParticleCollection, std::uint32_t parentParticle,
		const Effect* effect, ParticleEmissionPair emissionPair, EffectRuntimeContext runtimeContext,
		const ParticleEmitterEmissionData& emitterEmissionData,
		const ParticleTypeEmissionData& ptypeEmissionData);
};
}
