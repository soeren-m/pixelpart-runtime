#include "LifeSolver.h"

namespace pixelpart {
LifeSolver::LifeSolver() {

}

void LifeSolver::solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, uint32_t particleCount, float_t t, float_t dt) const {
	for(uint32_t p = 0u; p < particleCount; p++) {
		particles.life[p] += dt / particles.lifespan[p];
	}
}

void LifeSolver::prepare(const Effect& effect) {

}
}