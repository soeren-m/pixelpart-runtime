#pragma once

#include "../effect/Effect.h"
#include "ParticleData.h"

namespace pixelpart {
class ParticleSolver {
public:
	enum RefreshFlag : uint32_t {
		refresh_none = 0,
		refresh_force = 1,
		refresh_collision = 2
	};

	virtual void solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType, ParticleData& particles, uint32_t numParticles, float_t t, float_t dt) = 0;
	virtual void refresh(const Effect* effect, uint32_t flags) = 0;

	virtual uint32_t getNumActiveThreads() const = 0;
};
}