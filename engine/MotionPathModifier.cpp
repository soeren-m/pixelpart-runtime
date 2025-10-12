#include "MotionPathModifier.h"
#include "../common/Math.h"
#include "../effect/ParticleType.h"

namespace pixelpart {
void MotionPathModifier::apply(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
	const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const {
	const float_t positionLookahead = 0.1;
	const float_t targetLookahead = 0.01;

	const ParticleType& particleType = effect->particleTypes().at(particleTypeId);
	if(particleType.motionPathForce().value() < 0.1) {
		return;
	}

	for(std::uint32_t p = 0; p < particleCount; p++) {
		float3_t predictedPosition = particles.position[p] +
			particles.velocity[p] * positionLookahead +
			particles.force[p] * positionLookahead * positionLookahead;

		float3_t targetPosition = particleType.motionPath().at(particles.life[p] + targetLookahead);
		float3_t targetVelocity = targetPosition - predictedPosition;
		targetVelocity *= particleType.motionPathForce().value();

		particles.force[p] += targetVelocity - particles.velocity[p];
	}
}

void MotionPathModifier::reset(const Effect* effect, EffectRuntimeContext runtimeContext) {

}
}
