#pragma once

#include "ParticleSolver.h"

namespace pixelpart {
class RotationSolver : public ParticleSolver {
public:
	RotationSolver();

	virtual void solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		ParticleCollection::WritePtr particles, uint32_t numParticles, float_t t, float_t dt) const override;

	virtual void prepare(const Effect& effect) override;
};
}