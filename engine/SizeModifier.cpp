#include "SizeModifier.h"
#include "../common/Math.h"
#include "../effect/ParticleType.h"

namespace pixelpart {
void SizeModifier::apply(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
	const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const {
	const ParticleType& particleType = effect->particleTypes().at(particleTypeId);

	for(std::uint32_t p = 0; p < particleCount; p++) {
		particles.size[p] = particleType.size().at(particles.life[p]) * particles.initialSize[p];
		particles.size[p] += particleType.stretch().at(particles.life[p]) * particles.size[p] * glm::length(particles.velocity[p]);
	}
}

void SizeModifier::reset(const Effect* effect, EffectRuntimeContext runtimeContext) {

}
}
