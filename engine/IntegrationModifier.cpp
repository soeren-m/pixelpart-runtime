#include "IntegrationModifier.h"
#include "../common/Math.h"

namespace pixelpart {
void IntegrationModifier::run(const SceneGraph& sceneGraph, const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, uint32_t particleCount, const RuntimeContext& runtimeContext) const {
	float_t dt = runtimeContext.deltaTime();

	for(uint32_t p = 0u; p < particleCount; p++) {
		particles.velocity[p] += particles.force[p] * dt;
		particles.position[p] += particles.velocity[p] * dt;
	}

	if(particleType.positionRelative()) {
		float3_t origin = sceneGraph.globalTransform(particleEmitter.id(), runtimeContext).position();

		for(uint32_t p = 0u; p < particleCount; p++) {
			particles.globalPosition[p] = particles.position[p] + origin;
		}
	}
	else {
		for(uint32_t p = 0u; p < particleCount; p++) {
			particles.globalPosition[p] = particles.position[p];
		}
	}
}

void IntegrationModifier::prepare(const Effect& effect, const RuntimeContext& runtimeContext) {

}
}