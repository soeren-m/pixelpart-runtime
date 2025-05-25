#include "ParticleModifierPipeline.h"

namespace pixelpart {
void ParticleModifierPipeline::run(const Effect* effect, RuntimeContext runtimeContext, ParticleRuntimeId runtimeId,
	ParticleCollection::WritePtr particles, std::uint32_t particleCount) const {
	sizeModifier.run(effect, runtimeContext, runtimeId, particles, particleCount);
	colorModifier.run(effect, runtimeContext, runtimeId, particles, particleCount);
	accelerationModifier.run(effect, runtimeContext, runtimeId, particles, particleCount);
	forceModifier.run(effect, runtimeContext, runtimeId, particles, particleCount);
	collisionModifier.run(effect, runtimeContext, runtimeId, particles, particleCount);
	motionPathModifier.run(effect, runtimeContext, runtimeId, particles, particleCount);
	rotationModifier.run(effect, runtimeContext, runtimeId, particles, particleCount);
	integrationModifier.run(effect, runtimeContext, runtimeId, particles, particleCount);
	lifeModifier.run(effect, runtimeContext, runtimeId, particles, particleCount);
	coordinate2dModifier.run(effect, runtimeContext, runtimeId, particles, particleCount);
}

void ParticleModifierPipeline::prepare(const Effect& effect, const RuntimeContext& runtimeContext) {
	sizeModifier.prepare(effect, runtimeContext);
	colorModifier.prepare(effect, runtimeContext);
	accelerationModifier.prepare(effect, runtimeContext);
	forceModifier.prepare(effect, runtimeContext);
	collisionModifier.prepare(effect, runtimeContext);
	motionPathModifier.prepare(effect, runtimeContext);
	rotationModifier.prepare(effect, runtimeContext);
	integrationModifier.prepare(effect, runtimeContext);
	lifeModifier.prepare(effect, runtimeContext);
	coordinate2dModifier.prepare(effect, runtimeContext);
}
}