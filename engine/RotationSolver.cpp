#include "RotationSolver.h"
#include "../common/Math.h"

namespace pixelpart {
RotationSolver::RotationSolver() {

}

void RotationSolver::solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, uint32_t particleCount, float_t t, float_t dt) const {
	switch(particleType.rotationMode()) {
		case RotationMode::angle: {
			for(uint32_t p = 0u; p < particleCount; p++) {
				particles.rotation[p] = particles.initialRotation[p] + particleType.rotation().at(particles.life[p]);
				particles.rotation[p] += particleType.rotationBySpeed().at(particles.life[p]) * glm::length(particles.velocity[p]);
			}

			break;
		}

		case RotationMode::velocity: {
			for(uint32_t p = 0u; p < particleCount; p++) {
				particles.rotation[p] += (particles.initialAngularVelocity[p] + particleType.rotation().at(particles.life[p])) * dt;
				particles.rotation[p] += particleType.rotationBySpeed().at(particles.life[p]) * glm::length(particles.velocity[p]) * dt;
			}

			break;
		}

		default: {
			break;
		}
	}
}

void RotationSolver::prepare(const Effect& effect) {

}
}