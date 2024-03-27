#pragma once

#include "ParticleSolver.h"

namespace pixelpart {
class SizeSolver : public ParticleSolver {
public:
	SizeSolver();

	virtual void solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		ParticleDataPointer particles, uint32_t numParticles, float_t t, float_t dt) const override;

	virtual void refresh(const Effect& effect) override;
};
}