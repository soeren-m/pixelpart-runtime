#include "IntegrationSolver.h"
#include "../common/Math.h"
#include <cmath>

namespace pixelpart {
IntegrationSolver::IntegrationSolver() {

}

void IntegrationSolver::solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, uint32_t particleCount, float_t t, float_t dt) const {
	float_t particleEmitterLife = std::fmod(t - particleEmitter.lifetimeStart, particleEmitter.lifetimeDuration) / particleEmitter.lifetimeDuration;
	vec3_t particleEmitterPosition = particleEmitter.position.get(particleEmitterLife);

	for(uint32_t p = 0u; p < particleCount; p++) {
		particles.velocity[p] += particles.force[p] * dt;
		particles.position[p] += particles.velocity[p] * dt;

		particles.globalPosition[p] = particleType.positionRelative
			? particles.position[p] + particleEmitterPosition
			: particles.position[p];
	}
}

void IntegrationSolver::prepare(const Effect& effect) {

}
}