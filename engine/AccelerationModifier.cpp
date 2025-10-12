#include "AccelerationModifier.h"
#include "../common/Math.h"
#include "../effect/ParticleType.h"

namespace pixelpart {
void AccelerationModifier::apply(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
	const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const {
	const ParticleType& particleType = effect->particleTypes().at(particleTypeId);
	float3_t globalEmitterPosition = effect->sceneGraph().globalTransform(particleEmitterId, runtimeContext).position();

	for(std::uint32_t p = 0; p < particleCount; p++) {
		float3_t forwardDirection = (particles.velocity[p] != float3_t(0.0))
			? glm::normalize(particles.velocity[p])
			: float3_t(0.0);
		float3_t radialDirection = (globalEmitterPosition != particles.globalPosition[p])
			? glm::normalize(globalEmitterPosition - particles.globalPosition[p])
			: float3_t(0.0);

		particles.force[p] = forwardDirection * particleType.acceleration().at(particles.life[p]);
		particles.force[p] += radialDirection * particleType.radialAcceleration().at(particles.life[p]);
	}
}

void AccelerationModifier::reset(const Effect* effect, EffectRuntimeContext runtimeContext) {

}
}
