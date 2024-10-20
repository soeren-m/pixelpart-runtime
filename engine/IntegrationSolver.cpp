#include "IntegrationSolver.h"
#include "../common/Math.h"

namespace pixelpart {
IntegrationSolver::IntegrationSolver() {

}

void IntegrationSolver::solve(const SceneGraph& sceneGraph, const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, uint32_t particleCount, float_t t, float_t dt) const {
	float3_t emitterPosition = sceneGraph.globalTransform(particleEmitter, particleEmitter.life(t)).position();

	for(uint32_t p = 0u; p < particleCount; p++) {
		particles.velocity[p] += particles.force[p] * dt;
		particles.position[p] += particles.velocity[p] * dt;

		particles.globalPosition[p] = particleType.positionRelative()
			? particles.position[p] + emitterPosition
			: particles.position[p];
	}
}

void IntegrationSolver::prepare(const Effect& effect) {

}
}