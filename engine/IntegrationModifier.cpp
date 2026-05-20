#include "IntegrationModifier.h"
#include "../common/Types.h"
#include "../effect/ParticleType.h"
#include "../effect/ParticleEmitter.h"
#include "../effect/ParticleCoordinateSystem.h"
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

	if(particleType.coordinateSystem() == ParticleCoordinateSystem::local) {
		matrix4_t parentTransform = effect->sceneGraph().globalTransform(particleEmitter.id(), runtimeContext).matrix();

		for(std::uint32_t p = 0; p < particleCount; p++) {
			particles.globalPosition[p] = float3_t(parentTransform * float4_t(particles.position[p], 1.0));
		}
	}
	else {
		std::copy(particles.position, particles.position + particleCount, particles.globalPosition);
	}
}

void IntegrationModifier::reset(const Effect* effect, EffectRuntimeContext runtimeContext) {

}
}
