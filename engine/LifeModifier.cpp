#include "LifeModifier.h"

namespace pixelpart {
void LifeModifier::run(const SceneGraph& sceneGraph, const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, std::uint32_t particleCount, const RuntimeContext& runtimeContext) const {
	float_t dt = runtimeContext.deltaTime();

	for(std::uint32_t p = 0; p < particleCount; p++) {
		particles.life[p] += dt / particles.lifespan[p];
	}
}

void LifeModifier::prepare(const Effect& effect, const RuntimeContext& runtimeContext) {

}
}