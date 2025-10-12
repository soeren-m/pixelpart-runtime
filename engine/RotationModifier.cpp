#include "RotationModifier.h"
#include "../common/Math.h"
#include "../effect/ParticleType.h"

namespace pixelpart {
void RotationModifier::apply(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
	const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const {
	const ParticleType& particleType = effect->particleTypes().at(particleTypeId);
	float_t dt = runtimeContext.deltaTime();

	switch(particleType.rotationMode()) {
		case RotationMode::angle: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				particles.rotation[p] = particles.initialRotation[p] + particleType.rotation().at(particles.life[p]);
				particles.rotation[p] += particleType.rotationBySpeed().at(particles.life[p]) * glm::length(particles.velocity[p]);
			}

			break;
		}

		case RotationMode::velocity: {
			for(std::uint32_t p = 0; p < particleCount; p++) {
				particles.rotation[p] += (particles.initialAngularVelocity[p] + particleType.rotation().at(particles.life[p])) * dt;
				particles.rotation[p] += particleType.rotationBySpeed().at(particles.life[p]) * glm::length(particles.velocity[p]) * dt;
			}

			break;
		}

		default: {
			break;
		}
	}
}

void RotationModifier::reset(const Effect* effect, EffectRuntimeContext runtimeContext) {

}
}
