#include "Coordinate2dModifier.h"

namespace pixelpart {
void Coordinate2dModifier::run(const SceneGraph& sceneGraph, const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, uint32_t particleCount, const RuntimeContext& runtimeContext) const {
	if(effect3d) {
		return;
	}

	for(uint32_t p = 0; p < particleCount; p++) {
		particles.rotation[p].y = 0.0;
		particles.rotation[p].z = 0.0;
		particles.force[p].z = 0.0;
		particles.velocity[p].z = 0.0;
		particles.position[p].z = 0.0;
		particles.globalPosition[p].z = 0.0;
	}
}

void Coordinate2dModifier::prepare(const Effect& effect, const RuntimeContext& runtimeContext) {
	effect3d = effect.is3d();
}
}