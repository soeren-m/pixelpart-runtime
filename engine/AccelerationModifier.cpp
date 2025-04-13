#include "AccelerationModifier.h"
#include "../common/Math.h"

namespace pixelpart {
void AccelerationModifier::run(const SceneGraph& sceneGraph, const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, uint32_t particleCount, const RuntimeContext& runtimeContext) const {
	float3_t emitterPosition = sceneGraph.globalTransform(particleEmitter.id(), runtimeContext).position();

	for(uint32_t p = 0u; p < particleCount; p++) {
		float3_t forwardDirection = (particles.velocity[p] != float3_t(0.0))
			? glm::normalize(particles.velocity[p])
			: float3_t(0.0);
		float3_t radialDirection = (emitterPosition != particles.globalPosition[p])
			? glm::normalize(emitterPosition - particles.globalPosition[p])
			: float3_t(0.0);

		particles.force[p] = forwardDirection * particleType.acceleration().at(particles.life[p]);
		particles.force[p] += radialDirection * particleType.radialAcceleration().at(particles.life[p]);
	}
}

void AccelerationModifier::prepare(const Effect& effect, const RuntimeContext& runtimeContext) {

}
}