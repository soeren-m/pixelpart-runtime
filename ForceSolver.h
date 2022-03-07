#pragma once

#include "ForceField.h"
#include "ParticleData.h"

namespace pixelpart {
class ForceSolver {
public:
	ForceSolver();

	void solve(uint32_t emitterId, ParticleDataPointer& particles, uint32_t p, floatd particleWeight, floatd t, floatd dt) const;
	void solve(ParticleDataPointer& particles, uint32_t p, floatd particleWeight, floatd t, floatd dt, const ForceField& force) const;

	void update(const std::vector<ForceField>& allForces);

private:
	std::vector<ForceField> forces;
};
}