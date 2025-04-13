#include "SingleThreadedEffectEngine.h"
#include "../effect/ParticleRuntimePair.h"

namespace pixelpart {
SingleThreadedEffectEngine::SingleThreadedEffectEngine(const Effect& effect, uint32_t particleCapacity) : EffectEngine(),
	engineEffect(effect), engineParticleCapacity(particleCapacity), particleGenerator(effect, particleRuntimeInstances) {

}

void SingleThreadedEffectEngine::step(float_t dt) {
	engineDeltaTime = dt;
	RuntimeContext rtContext = runtimeContext();

	particleRuntimeInstances.match(engineEffect, engineParticleCapacity);

	particleGenerator.generate(rtContext);
	particleModifierPipeline.prepare(engineEffect, rtContext);

	for(ParticleRuntimeInstance& runtimeInstance : particleRuntimeInstances) {
		const ParticleType& particleType = engineEffect.particleTypes().at(runtimeInstance.typeId());
		const ParticleEmitter& particleEmitter = engineEffect.sceneGraph().at<ParticleEmitter>(runtimeInstance.emitterId());

		runtimeInstance.particles().removeDead();

		ParticleCollection::WritePtr particles = runtimeInstance.particles().writePtr();
		uint32_t particleCount = runtimeInstance.particles().count();

		particleModifierPipeline.run(
			engineEffect.sceneGraph(), particleEmitter, particleType,
			particles, particleCount, rtContext);
	}

	engineTime += dt;
}
void SingleThreadedEffectEngine::restart(bool reset) {
	engineTime = 0.0;
	engineDeltaTime = 0.0;
	triggerActivationTimes.clear();

	if(reset) {
		particleGenerator.reset();
	}
}

float_t SingleThreadedEffectEngine::currentTime() const {
	return engineTime;
}
RuntimeContext SingleThreadedEffectEngine::runtimeContext() const {
	return RuntimeContext(engineTime, engineDeltaTime, triggerActivationTimes);
}

void SingleThreadedEffectEngine::seed(uint32_t seed) {
	particleGenerator.seed(seed);
}

void SingleThreadedEffectEngine::activateTrigger(id_t triggerId) {
	if(engineEffect.triggers().count(triggerId) == 0 ||
		triggerActivationTimes.count(triggerId) != 0) {
		return;
	}

	triggerActivationTimes[triggerId] = engineTime;
}

void SingleThreadedEffectEngine::spawnParticles(id_t particleEmitterId, uint32_t count, float_t time) {
	for(ParticleRuntimeInstance& runtimeInstance : particleRuntimeInstances) {
		if(runtimeInstance.emitterId() != particleEmitterId) {
			continue;
		}

		const ParticleEmitter& particleEmitter = engineEffect.sceneGraph().at<ParticleEmitter>(runtimeInstance.emitterId());
		if(!particleEmitter.primary()) {
			continue;
		}

		particleGenerator.generate(particleEmitterId, runtimeInstance.typeId(), count, time);
	}
}
void SingleThreadedEffectEngine::spawnParticles(id_t particleEmitterId, id_t particleTypeId, uint32_t count, float_t time) {
	ParticleRuntimeInstance* runtimeInstance = particleRuntimeInstances.find(particleEmitterId, particleTypeId);
	if(!runtimeInstance) {
		return;
	}

	const ParticleEmitter& particleEmitter = engineEffect.sceneGraph().at<ParticleEmitter>(runtimeInstance->emitterId());
	if(!particleEmitter.primary()) {
		return;
	}

	particleGenerator.generate(particleEmitterId, particleTypeId, count, time);
}

uint32_t SingleThreadedEffectEngine::particleCount() const {
	uint32_t count = 0;
	for(const ParticleRuntimeInstance& runtimeInstance : particleRuntimeInstances) {
		count += runtimeInstance.particles().count();
	}

	return count;
}
uint32_t SingleThreadedEffectEngine::particleCount(id_t particleEmitterId, id_t particleTypeId) const {
	const ParticleRuntimeInstance* runtimeInstance = particleRuntimeInstances.find(particleEmitterId, particleTypeId);
	if(!runtimeInstance) {
		return 0;
	}

	return runtimeInstance->particles().count();
}

const ParticleCollection* SingleThreadedEffectEngine::particles(id_t particleEmitterId, id_t particleTypeId) const {
	const ParticleRuntimeInstance* runtimeInstance = particleRuntimeInstances.find(particleEmitterId, particleTypeId);
	if(!runtimeInstance) {
		return nullptr;
	}

	return &runtimeInstance->particles();
}

const Effect& SingleThreadedEffectEngine::effect() const {
	return engineEffect;
}

uint32_t SingleThreadedEffectEngine::particleCapacity() const {
	return engineParticleCapacity;
}

}