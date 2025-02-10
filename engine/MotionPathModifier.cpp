#include "MotionPathModifier.h"
#include "../common/Math.h"

namespace pixelpart {
void MotionPathModifier::run(const SceneGraph& sceneGraph, const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, uint32_t particleCount, float_t t, float_t dt) const {
	const float_t positionLookahead = 0.1;
	const float_t targetLookahead = 0.01;

	if(particleType.motionPathForce().value() < 0.1) {
		return;
	}

	for(uint32_t p = 0u; p < particleCount; p++) {
		float3_t predictedPosition = particles.position[p] +
			particles.velocity[p] * positionLookahead +
			particles.force[p] * positionLookahead * positionLookahead;

		float3_t targetPosition = particleType.motionPath().at(particles.life[p] + targetLookahead);
		float3_t targetVelocity = targetPosition - predictedPosition;
		targetVelocity *= particleType.motionPathForce().value();

		particles.force[p] += targetVelocity - particles.velocity[p];
	}
}

void MotionPathModifier::prepare(const Effect& effect, float_t t) {

}
}