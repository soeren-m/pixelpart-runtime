#include "AccelerationSolver.h"
#include "../common/Math.h"
#include <cmath>

namespace pixelpart {
AccelerationSolver::AccelerationSolver() {

}

void AccelerationSolver::solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, uint32_t particleCount, float_t t, float_t dt) const {
	float_t particleEmitterLife = std::fmod(t - particleEmitter.start(), particleEmitter.duration()) / particleEmitter.duration();
	vec3_t particleEmitterPosition = particleEmitter.position().at(particleEmitterLife);

	for(uint32_t p = 0u; p < particleCount; p++) {
		vec3_t forwardDirection = (particles.velocity[p] != vec3_t(0.0))
			? glm::normalize(particles.velocity[p])
			: vec3_t(0.0);
		vec3_t radialDirection = (particleEmitterPosition != particles.globalPosition[p])
			? glm::normalize(particleEmitterPosition - particles.globalPosition[p])
			: vec3_t(0.0);

		particles.force[p] = forwardDirection * particleType.acceleration().at(particles.life[p]);
		particles.force[p] += radialDirection * particleType.radialAcceleration().at(particles.life[p]);
	}
}

void AccelerationSolver::prepare(const Effect& effect) {

}
}