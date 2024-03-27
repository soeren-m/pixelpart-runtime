#include "RotationSolver.h"

namespace pixelpart {
RotationSolver::RotationSolver() {

}

void RotationSolver::solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleDataPointer particles, uint32_t numParticles, float_t t, float_t dt) const {
	switch(particleType.rotationMode) {
		case RotationMode::angle: {
			for(uint32_t p = 0u; p < numParticles; p++) {
				particles.rotation[p] = particles.initialRotation[p] + particleType.rotation.get(particles.life[p]);
				particles.rotation[p] += particleType.rotationBySpeed.get(particles.life[p]) * glm::length(particles.velocity[p]);
			}

			break;
		}

		case RotationMode::velocity: {
			for(uint32_t p = 0u; p < numParticles; p++) {
				particles.rotation[p] += (particles.initialAngularVelocity[p] + particleType.rotation.get(particles.life[p])) * dt;
				particles.rotation[p] += particleType.rotationBySpeed.get(particles.life[p]) * glm::length(particles.velocity[p]) * dt;
			}

			break;
		}

		default: {
			break;
		}
	}
}

void RotationSolver::refresh(const Effect& effect) {

}
}