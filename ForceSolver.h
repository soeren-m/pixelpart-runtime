#pragma once

#include "Effect.h"
#include "ParticleData.h"
#include "ParticleType.h"
#include <bitset>

namespace pixelpart {
class ForceSolver {
public:
	ForceSolver();

	void solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t particleIndex, floatd particleWeight, floatd t, floatd dt) const;
	void solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t particleIndex, floatd particleWeight, floatd t, floatd dt, const ForceField& forceField) const;

	void update(const Effect* effect);

private:
	std::vector<ForceField> forceFields;
	std::vector<std::bitset<256>> forceFieldExclusionSets;
};
}