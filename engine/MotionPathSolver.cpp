#include "MotionPathSolver.h"

namespace pixelpart {
MotionPathSolver::MotionPathSolver() {

}

void MotionPathSolver::solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t numParticles, floatd t, floatd dt) const {
	const floatd positionLookahead = 0.1;
	const floatd targetLookahead = 0.01;

	if(particleType.motionPathForce < 0.1) {
		return;
	}

	for(uint32_t p = 0; p < numParticles; p++) {
		vec3d predictedPosition = particles.position[p] +
			particles.velocity[p] * positionLookahead +
			particles.force[p] * positionLookahead * positionLookahead;

		vec3d targetPosition = particleType.position.get(particles.life[p] + targetLookahead);
		vec3d targetVelocity = targetPosition - predictedPosition;
		targetVelocity *= particleType.motionPathForce;

		particles.force[p] += targetVelocity - particles.velocity[p];
	}
}
}