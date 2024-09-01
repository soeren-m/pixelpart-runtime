#pragma once

#include "ParticleCollection.h"
#include "../effect/Effect.h"

namespace pixelpart {
class ParticleSolver {
public:
	virtual ~ParticleSolver() = default;

	virtual void solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		ParticleCollection::WritePtr particles, uint32_t particleCount, float_t t, float_t dt) const = 0;

	virtual void prepare(const Effect& effect) = 0;
};
}