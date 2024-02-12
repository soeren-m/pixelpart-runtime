#pragma once

#include "../effect/Effect.h"
#include "ParticleData.h"

namespace pixelpart {
class MotionPathSolver {
public:
	MotionPathSolver();

	void solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t numParticles, float_t t, float_t dt) const;
};
}