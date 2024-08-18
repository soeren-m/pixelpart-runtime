#include "SizeSolver.h"
#include "../common/Math.h"

namespace pixelpart {
SizeSolver::SizeSolver() {

}

void SizeSolver::solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, uint32_t numParticles, float_t t, float_t dt) const {
	for(uint32_t p = 0u; p < numParticles; p++) {
		particles.size[p] = particleType.size.get(particles.life[p]) * particles.initialSize[p];
		particles.size[p] += particleType.stretch.get(particles.life[p]) * particles.size[p] * glm::length(particles.velocity[p]);
	}
}

void SizeSolver::prepare(const Effect& effect) {

}
}