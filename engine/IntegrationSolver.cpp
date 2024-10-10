#include "IntegrationSolver.h"
#include "../common/Math.h"

namespace pixelpart {
IntegrationSolver::IntegrationSolver() {

}

void IntegrationSolver::solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, uint32_t particleCount, float_t t, float_t dt) const {
	float3_t particleEmitterPosition = particleEmitter.position().at(particleEmitter.life(t));

	for(uint32_t p = 0u; p < particleCount; p++) {
		particles.velocity[p] += particles.force[p] * dt;
		particles.position[p] += particles.velocity[p] * dt;

		particles.globalPosition[p] = particleType.positionRelative()
			? particles.position[p] + particleEmitterPosition
			: particles.position[p];
	}
}

void IntegrationSolver::prepare(const Effect& effect) {

}
}