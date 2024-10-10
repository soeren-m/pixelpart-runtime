#pragma once

#include "ParticleSolver.h"

namespace pixelpart {
class ColorSolver : public ParticleSolver {
public:
	ColorSolver();

	virtual void solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		ParticleCollection::WritePtr particles, uint32_t particleCount, float_t t, float_t dt) const override;

	virtual void prepare(const Effect& effect) override;
};
}