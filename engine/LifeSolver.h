#pragma once

#include "ParticleSolver.h"

namespace pixelpart {
class LifeSolver : public ParticleSolver {
public:
	LifeSolver();

	virtual void solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		ParticleWritePtr particles, uint32_t numParticles, float_t t, float_t dt) const override;

	virtual void refresh(const Effect& effect) override;
};
}