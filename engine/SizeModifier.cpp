#include "SizeModifier.h"
#include "../common/Math.h"

namespace pixelpart {
void SizeModifier::run(const SceneGraph& sceneGraph, const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, std::uint32_t particleCount, const RuntimeContext& runtimeContext) const {
	for(std::uint32_t p = 0; p < particleCount; p++) {
		particles.size[p] = particleType.size().at(particles.life[p]) * particles.initialSize[p];
		particles.size[p] += particleType.stretch().at(particles.life[p]) * particles.size[p] * glm::length(particles.velocity[p]);
	}
}

void SizeModifier::prepare(const Effect& effect, const RuntimeContext& runtimeContext) {

}
}