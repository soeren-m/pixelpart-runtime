#include "DefaultParticleModifier.h"

namespace pixelpart {
void DefaultParticleModifier::apply(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
	const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const {
	sizeModifier.apply(particles, particleCount, effect, particleEmitterId, particleTypeId, runtimeContext);
	colorModifier.apply(particles, particleCount, effect, particleEmitterId, particleTypeId, runtimeContext);
	accelerationModifier.apply(particles, particleCount, effect, particleEmitterId, particleTypeId, runtimeContext);
	forceModifier.apply(particles, particleCount, effect, particleEmitterId, particleTypeId, runtimeContext);
	collisionModifier.apply(particles, particleCount, effect, particleEmitterId, particleTypeId, runtimeContext);
	motionPathModifier.apply(particles, particleCount, effect, particleEmitterId, particleTypeId, runtimeContext);
	rotationModifier.apply(particles, particleCount, effect, particleEmitterId, particleTypeId, runtimeContext);
	coordinate2dModifier.apply(particles, particleCount, effect, particleEmitterId, particleTypeId, runtimeContext);
	integrationModifier.apply(particles, particleCount, effect, particleEmitterId, particleTypeId, runtimeContext);
	lifeModifier.apply(particles, particleCount, effect, particleEmitterId, particleTypeId, runtimeContext);
}

void DefaultParticleModifier::reset(const Effect* effect, EffectRuntimeContext runtimeContext) {
	sizeModifier.reset(effect, runtimeContext);
	colorModifier.reset(effect, runtimeContext);
	accelerationModifier.reset(effect, runtimeContext);
	forceModifier.reset(effect, runtimeContext);
	collisionModifier.reset(effect, runtimeContext);
	motionPathModifier.reset(effect, runtimeContext);
	rotationModifier.reset(effect, runtimeContext);
	coordinate2dModifier.reset(effect, runtimeContext);
	integrationModifier.reset(effect, runtimeContext);
	lifeModifier.reset(effect, runtimeContext);
}
}
