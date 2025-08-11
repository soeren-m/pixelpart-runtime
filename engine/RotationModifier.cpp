#include "RotationModifier.h"
#include "../common/Math.h"
#include "../effect/ParticleType.h"

namespace pixelpart {
void RotationModifier::run(const Effect* effect, RuntimeContext runtimeContext, ParticleRuntimeId runtimeId,
	ParticleCollection::WritePtr particles, std::uint32_t particleCount) const {
	const ParticleType& particleType = effect->particleTypes().at(runtimeId.typeId);
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

void RotationModifier::prepare(const Effect& effect, const RuntimeContext& runtimeContext) {

}
}
