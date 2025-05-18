#include "ParticleModifierPipeline.h"

namespace pixelpart {
void ParticleModifierPipeline::run(const SceneGraph& sceneGraph, const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, std::uint32_t particleCount, const RuntimeContext& runtimeContext) const {
	sizeModifier.run(sceneGraph, particleEmitter, particleType, particles, particleCount, runtimeContext);
	colorModifier.run(sceneGraph, particleEmitter, particleType, particles, particleCount, runtimeContext);
	accelerationModifier.run(sceneGraph, particleEmitter, particleType, particles, particleCount, runtimeContext);
	forceModifier.run(sceneGraph, particleEmitter, particleType, particles, particleCount, runtimeContext);
	collisionModifier.run(sceneGraph, particleEmitter, particleType, particles, particleCount, runtimeContext);
	motionPathModifier.run(sceneGraph, particleEmitter, particleType, particles, particleCount, runtimeContext);
	rotationModifier.run(sceneGraph, particleEmitter, particleType, particles, particleCount, runtimeContext);
	integrationModifier.run(sceneGraph, particleEmitter, particleType, particles, particleCount, runtimeContext);
	lifeModifier.run(sceneGraph, particleEmitter, particleType, particles, particleCount, runtimeContext);
	coordinate2dModifier.run(sceneGraph, particleEmitter, particleType, particles, particleCount, runtimeContext);
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