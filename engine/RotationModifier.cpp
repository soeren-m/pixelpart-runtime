#include "RotationModifier.h"
#include "../common/Math.h"

namespace pixelpart {
void RotationModifier::run(const SceneGraph& sceneGraph, const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, uint32_t particleCount, const RuntimeContext& runtimeContext) const {
	float_t dt = runtimeContext.deltaTime();

	switch(particleType.rotationMode()) {
		case RotationMode::angle: {
			for(uint32_t p = 0u; p < particleCount; p++) {
				particles.rotation[p] = particles.initialRotation[p] + particleType.rotation().at(particles.life[p]);
				particles.rotation[p] += particleType.rotationBySpeed().at(particles.life[p]) * glm::length(particles.velocity[p]);
			}

			break;
		}

		case RotationMode::velocity: {
			for(uint32_t p = 0u; p < particleCount; p++) {
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