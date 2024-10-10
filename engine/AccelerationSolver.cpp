#include "AccelerationSolver.h"
#include "../common/Math.h"

namespace pixelpart {
AccelerationSolver::AccelerationSolver() {

}

void AccelerationSolver::solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, uint32_t particleCount, float_t t, float_t dt) const {
	float3_t particleEmitterPosition = particleEmitter.position().at(particleEmitter.life(t));

	for(uint32_t p = 0u; p < particleCount; p++) {
		float3_t forwardDirection = (particles.velocity[p] != float3_t(0.0))
			? glm::normalize(particles.velocity[p])
			: float3_t(0.0);
		float3_t radialDirection = (particleEmitterPosition != particles.globalPosition[p])
			? glm::normalize(particleEmitterPosition - particles.globalPosition[p])
			: float3_t(0.0);

		particles.force[p] = forwardDirection * particleType.acceleration().at(particles.life[p]);
		particles.force[p] += radialDirection * particleType.radialAcceleration().at(particles.life[p]);
	}
}

void AccelerationSolver::prepare(const Effect& effect) {

}
}