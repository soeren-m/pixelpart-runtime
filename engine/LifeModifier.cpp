#include "LifeModifier.h"

namespace pixelpart {
void LifeModifier::run(const SceneGraph& sceneGraph, const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, uint32_t particleCount, const RuntimeContext& runtimeContext) const {
	float_t dt = runtimeContext.deltaTime();

	for(uint32_t p = 0u; p < particleCount; p++) {
		particles.life[p] += dt / particles.lifespan[p];
	}
}

void LifeModifier::prepare(const Effect& effect, const RuntimeContext& runtimeContext) {

}
}