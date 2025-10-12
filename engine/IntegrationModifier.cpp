#include "IntegrationModifier.h"
#include "../common/Math.h"
#include "../effect/ParticleType.h"
#include "../effect/ParticleEmitter.h"
#include <algorithm>

namespace pixelpart {
void IntegrationModifier::apply(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
	const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const {
	const ParticleType& particleType = effect->particleTypes().at(particleTypeId);
	const ParticleEmitter& particleEmitter = effect->sceneGraph().at<ParticleEmitter>(particleEmitterId);
	float_t dt = runtimeContext.deltaTime();

	for(std::uint32_t p = 0; p < particleCount; p++) {
		particles.velocity[p] += particles.force[p] * dt;
		particles.position[p] += particles.velocity[p] * dt;
	}

	std::copy(particles.position, particles.position + particleCount, particles.globalPosition);

	if(particleType.positionRelative()) {
		float3_t origin = effect->sceneGraph().globalTransform(particleEmitter.id(), runtimeContext).position();

		for(std::uint32_t p = 0; p < particleCount; p++) {
			particles.globalPosition[p] += origin;
		}
	}
}

void IntegrationModifier::reset(const Effect* effect, EffectRuntimeContext runtimeContext) {

}
}
