#pragma once

#include "../common/Types.h"
#include "../effect/Effect.h"
#include "../effect/SceneGraph.h"
#include "../effect/ParticleEmitter.h"
#include "../effect/ParticleType.h"
#include "ParticleCollection.h"

namespace pixelpart {
class ParticleModifier {
public:
	virtual ~ParticleModifier() = default;

	virtual void run(const SceneGraph& sceneGraph, const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		ParticleCollection::WritePtr particles, uint32_t particleCount, float_t t, float_t dt) const = 0;

	virtual void prepare(const Effect& effect, float_t t) = 0;
};
}