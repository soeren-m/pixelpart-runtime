#include "AccelerationSolver.h"

namespace pixelpart {
AccelerationSolver::AccelerationSolver() {

}

void AccelerationSolver::solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleDataPointer particles, uint32_t numParticles, float_t t, float_t dt) const {
	float_t particleEmitterLife = std::fmod(t - particleEmitter.lifetimeStart, particleEmitter.lifetimeDuration) / particleEmitter.lifetimeDuration;
	vec3_t particleEmitterPosition = particleEmitter.position.get(particleEmitterLife);

	for(uint32_t p = 0u; p < numParticles; p++) {
		vec3_t forwardDirection = (particles.velocity[p] != vec3_t(0.0))
			? glm::normalize(particles.velocity[p])
			: vec3_t(0.0);
		vec3_t radialDirection = (particleEmitterPosition != particles.globalPosition[p])
			? glm::normalize(particleEmitterPosition - particles.globalPosition[p])
			: vec3_t(0.0);

		particles.force[p] = forwardDirection * particleType.acceleration.get(particles.life[p]);
		particles.force[p] += radialDirection * particleType.radialAcceleration.get(particles.life[p]);
	}
}

void AccelerationSolver::refresh(const Effect& effect) {

}
}