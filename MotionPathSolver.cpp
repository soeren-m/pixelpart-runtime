#include "MotionPathSolver.h"

namespace pixelpart {
MotionPathSolver::MotionPathSolver() {

}

void MotionPathSolver::solve(const ParticleType& particleType, ParticleDataPointer& particles, uint32_t particleIndex, floatd t, floatd dt) {
	if(particleType.motionPathForce < 0.1) {
		return;
	}

	const floatd positionLookahead = 0.1;
	const floatd targetLookahead = 0.01;

	vec3d predictedPosition = particles.position[particleIndex] +
		particles.velocity[particleIndex] * positionLookahead +
		particles.force[particleIndex] * positionLookahead * positionLookahead;

	vec3d targetPosition = particleType.position.get(particles.life[particleIndex] + targetLookahead);
	vec3d targetVelocity = targetPosition - predictedPosition;
	targetVelocity *= particleType.motionPathForce;

	particles.force[particleIndex] += targetVelocity - particles.velocity[particleIndex];
}
}