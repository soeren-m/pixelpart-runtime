#include "LifeSolver.h"

namespace pixelpart {
LifeSolver::LifeSolver() {

}

void LifeSolver::solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleWritePtr particles, uint32_t numParticles, float_t t, float_t dt) const {
	for(uint32_t p = 0u; p < numParticles; p++) {
		particles.life[p] += dt / particles.lifespan[p];
	}
}

void LifeSolver::refresh(const Effect& effect) {

}
}