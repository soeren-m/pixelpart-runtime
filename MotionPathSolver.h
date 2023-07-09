#pragma once

#include "Effect.h"
#include "ParticleData.h"
#include "ParticleType.h"

namespace pixelpart {
class MotionPathSolver {
public:
	MotionPathSolver();

	void solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t numParticles, floatd t, floatd dt) const;
};
}