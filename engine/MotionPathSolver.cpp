#include "MotionPathSolver.h"

namespace pixelpart {
MotionPathSolver::MotionPathSolver() {

}

void MotionPathSolver::solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t numParticles, float_t t, float_t dt) const {
	const float_t positionLookahead = 0.1;
	const float_t targetLookahead = 0.01;

	if(particleType.motionPathForce.get() < 0.1) {
		return;
	}

	for(uint32_t p = 0u; p < numParticles; p++) {
		vec3_t predictedPosition = particles.position[p] +
			particles.velocity[p] * positionLookahead +
			particles.force[p] * positionLookahead * positionLookahead;

		vec3_t targetPosition = particleType.position.get(particles.life[p] + targetLookahead);
		vec3_t targetVelocity = targetPosition - predictedPosition;
		targetVelocity *= particleType.motionPathForce.get();

		particles.force[p] += targetVelocity - particles.velocity[p];
	}
}
}