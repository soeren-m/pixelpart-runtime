#include "ColorSolver.h"

namespace pixelpart {
ColorSolver::ColorSolver() {

}

void ColorSolver::solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleDataPointer& particles, uint32_t numParticles, float_t t, float_t dt) const {
	for(uint32_t p = 0u; p < numParticles; p++) {
		particles.color[p] = hsvAdd(
			particleType.color.get(particles.life[p]),
			particles.initialColor[p],
			particleType.opacity.get(particles.life[p]));
	}
}

void ColorSolver::refresh(const Effect& effect) {

}
}