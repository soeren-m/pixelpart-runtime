#pragma once

#include "../effect/Effect.h"
#include "ParticleData.h"

namespace pixelpart {
class ParticleSolver {
public:
	virtual void solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		ParticleData& particles, uint32_t numParticles, float_t t, float_t dt) = 0;

	virtual void refresh(const Effect& effect) = 0;
};
}