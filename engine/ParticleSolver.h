#pragma once

#include "ParticleDataPointer.h"
#include "../effect/Effect.h"

namespace pixelpart {
class ParticleSolver {
public:
	virtual void solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		ParticleDataPointer particles, uint32_t numParticles, float_t t, float_t dt) const = 0;

	virtual void refresh(const Effect& effect) = 0;
};
}