#include "AccelerationModifier.h"
#include "../common/Math.h"
#include "../effect/ParticleType.h"

namespace pixelpart {
void AccelerationModifier::run(const Effect* effect, RuntimeContext runtimeContext, ParticleRuntimeId runtimeId,
	ParticleCollection::WritePtr particles, std::uint32_t particleCount) const {
	const ParticleType& particleType = effect->particleTypes().at(runtimeId.typeId);
	float3_t globalEmitterPosition = effect->sceneGraph().globalTransform(runtimeId.emitterId, runtimeContext).position();

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

void AccelerationModifier::prepare(const Effect& effect, const RuntimeContext& runtimeContext) {

}
}
