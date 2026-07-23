#include "MotionPathModifier.h"
#include "../effect/Curve.h"
#include "../effect/ParticleType.h"
#include "../types/Types.h"
#include <cmath>

namespace pixelpart {
void MotionPathModifier::apply(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
	const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const {
	const ParticleType& particleType = effect->particleTypes().at(particleTypeId);

	const Curve<float3_t>& particleMotionPath = particleType.motionPath().resultCurve();
	float_t springCoeff = particleType.motionPathForce().value();
	float_t dampingCoeff = 2.0 * std::sqrt(springCoeff);

	if(springCoeff < 0.1) {
		return;
	}

	for(std::uint32_t p = 0; p < particleCount; p++) {
		float3_t pathPosition = particleMotionPath.at(particles.life[p]);

		particles.force[p] += (pathPosition - particles.position[p]) * springCoeff;
		particles.force[p] -= particles.velocity[p] * dampingCoeff;
	}
}

void MotionPathModifier::reset(const Effect* effect, EffectRuntimeContext runtimeContext) {

}
}
