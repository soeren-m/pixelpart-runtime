#include "Coordinate2dModifier.h"

namespace pixelpart {
void Coordinate2dModifier::apply(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
	const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const {
	if(effect->is3d()) {
		return;
	}

	for(std::uint32_t p = 0; p < particleCount; p++) {
		particles.rotation[p].y = 0.0;
		particles.rotation[p].z = 0.0;
		particles.force[p].z = 0.0;
		particles.velocity[p].z = 0.0;
	}
}

void Coordinate2dModifier::reset(const Effect* effect, EffectRuntimeContext runtimeContext) {

}
}
