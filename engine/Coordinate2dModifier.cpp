#include "Coordinate2dModifier.h"

namespace pixelpart {
void Coordinate2dModifier::run(const Effect* effect, RuntimeContext runtimeContext, ParticleRuntimeId runtimeId,
	ParticleCollection::WritePtr particles, std::uint32_t particleCount) const {
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

void Coordinate2dModifier::prepare(const Effect& effect, const RuntimeContext& runtimeContext) {

}
}