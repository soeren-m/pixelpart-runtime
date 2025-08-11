#include "SizeModifier.h"
#include "../common/Math.h"
#include "../effect/ParticleType.h"

namespace pixelpart {
void SizeModifier::run(const Effect* effect, RuntimeContext runtimeContext, ParticleRuntimeId runtimeId,
	ParticleCollection::WritePtr particles, std::uint32_t particleCount) const {
	const ParticleType& particleType = effect->particleTypes().at(runtimeId.typeId);

	for(std::uint32_t p = 0; p < particleCount; p++) {
		particles.size[p] = particleType.size().at(particles.life[p]) * particles.initialSize[p];
		particles.size[p] += particleType.stretch().at(particles.life[p]) * particles.size[p] * glm::length(particles.velocity[p]);
	}
}

void SizeModifier::prepare(const Effect& effect, const RuntimeContext& runtimeContext) {

}
}
