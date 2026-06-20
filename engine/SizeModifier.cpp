#include "SizeModifier.h"
#include "../common/Types.h"
#include "../common/Curve.h"
#include "../math/Geometry.h"
#include "../effect/ParticleType.h"

namespace pixelpart {
void SizeModifier::apply(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
	const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const {
	const ParticleType& particleType = effect->particleTypes().at(particleTypeId);

	const Curve<float3_t>& particleSizeCurve = particleType.size().resultCurve();
	const Curve<float3_t>& particleStretchCurve = particleType.stretch().resultCurve();

	for(std::uint32_t p = 0; p < particleCount; p++) {
		particles.size[p] = particleSizeCurve.at(particles.life[p]) * particles.initialSize[p];
		particles.size[p] += particleStretchCurve.at(particles.life[p]) * particles.size[p] * math::length(particles.velocity[p]);
	}
}

void SizeModifier::reset(const Effect* effect, EffectRuntimeContext runtimeContext) {

}
}
