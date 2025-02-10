#include "LifeModifier.h"

namespace pixelpart {
void LifeModifier::run(const SceneGraph& sceneGraph, const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, uint32_t particleCount, float_t t, float_t dt) const {
	for(uint32_t p = 0u; p < particleCount; p++) {
		particles.life[p] += dt / particles.lifespan[p];
	}
}

void LifeModifier::prepare(const Effect& effect, float_t t) {

}
}