#pragma once

#include "Effect.h"
#include "ParticleData.h"
#include "ParticleType.h"

namespace pixelpart {
class MotionPathSolver {
public:
	MotionPathSolver();

	static void solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t particleIndex, floatd t, floatd dt);
};
}