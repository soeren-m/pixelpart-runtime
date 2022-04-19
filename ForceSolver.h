#pragma once

#include "ForceField.h"
#include "ParticleData.h"
#include "ParticleEmitter.h"

namespace pixelpart {
class ForceSolver {
public:
	ForceSolver();

	void solve(const ParticleEmitter& emitter, ParticleDataPointer& particles, uint32_t p, floatd particleWeight, floatd t, floatd dt) const;
	void solve(ParticleDataPointer& particles, uint32_t p, floatd particleWeight, floatd t, floatd dt, const ForceField& force) const;

	void update(const std::vector<ForceField>& allForces);

private:
	std::vector<ForceField> forces;
};
}