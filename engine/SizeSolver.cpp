#include "SizeSolver.h"
#include "../common/Math.h"

namespace pixelpart {
SizeSolver::SizeSolver() {

}

void SizeSolver::solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, uint32_t particleCount, float_t t, float_t dt) const {
	for(uint32_t p = 0u; p < particleCount; p++) {
		particles.size[p] = particleType.size().at(particles.life[p]) * particles.initialSize[p];
		particles.size[p] += particleType.stretch().at(particles.life[p]) * particles.size[p] * glm::length(particles.velocity[p]);
	}
}

void SizeSolver::prepare(const Effect& effect) {

}
}