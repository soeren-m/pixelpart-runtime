#include "SingleThreadedEffectEngine.h"
#include "ParticleCollection.h"
#include "ParticleEmissionState.h"
#include "../common/MapSync.h"
#include "../effect/ParticleEmissionPair.h"
#include <algorithm>

namespace pixelpart {
SingleThreadedEffectEngine::SingleThreadedEffectEngine(const Effect& effect,
	std::shared_ptr<ParticleGenerator> particleGenerator, std::shared_ptr<ParticleModifier> particleModifier,
	std::uint32_t particleCapacity) : EffectEngine(),
	engineEffect(effect),
	engineParticleGenerator(particleGenerator),
	engineParticleModifier(particleModifier),
	engineParticleCapacity(particleCapacity) {

}
SingleThreadedEffectEngine::SingleThreadedEffectEngine(const Effect& effect,
	std::shared_ptr<ParticleGenerator> particleGenerator,
	std::shared_ptr<ParticleModifier> particleModifier,
	std::uint32_t particleCapacity,
	const EffectRuntimeState& initialState,
	const EffectRuntimeContext& initialContext) : EffectEngine(),
	engineEffect(effect),
	engineParticleGenerator(particleGenerator),
	engineParticleModifier(particleModifier),
	engineParticleCapacity(particleCapacity),
	engineState(initialState),
	engineContext(initialContext) {

}

void SingleThreadedEffectEngine::advance(float_t dt) {
	dt = std::max(dt, 0.0);
	engineContext.deltaTime() = dt;

	syncMapToKeys(engineState.particleCollections(), engineEffect.particleEmissionPairs(), engineParticleCapacity);
	syncMapToKeys(engineState.particleEmissionStates(), engineEffect.particleEmissionPairs());

	engineParticleGenerator->generate(engineState, &engineEffect, engineContext);

	engineParticleModifier->reset(&engineEffect, engineContext);

	for(auto& collectionEntry : engineState.particleCollections()) {
		ParticleEmissionPair emissionPair = collectionEntry.first;
		ParticleCollection& particleCollection = collectionEntry.second;

		engineParticleModifier->apply(
			particleCollection.writePtr(),
			particleCollection.count(),
			&engineEffect,
			emissionPair.emitterId, emissionPair.typeId,
			engineContext);
	}

	engineContext.time() += dt;
}
void SingleThreadedEffectEngine::restart() {
	engineContext.time() = 0.0;
	engineContext.deltaTime() = 0.0;
	engineContext.triggerActivationTimes().clear();
}
void SingleThreadedEffectEngine::reseed(std::uint32_t seed) {
	engineState.rng() = std::mt19937(seed);
}

void SingleThreadedEffectEngine::generateParticles(std::uint32_t count, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) {
	engineParticleGenerator->generate(engineState,
		count, &engineEffect, particleEmitterId, particleTypeId, runtimeContext);
}
void SingleThreadedEffectEngine::clearParticles() {
	engineParticleGenerator->clear(engineState);
}

void SingleThreadedEffectEngine::activateTrigger(id_t triggerId) {
	if(engineEffect.triggers().count(triggerId) == 0 ||
		engineContext.triggerActivationTimes().count(triggerId) != 0) {
		return;
	}

	engineContext.triggerActivationTimes()[triggerId] = engineContext.time() + engineContext.deltaTime() * 0.5;
}

const Effect& SingleThreadedEffectEngine::effect() const {
	return engineEffect;
}

const EffectRuntimeState& SingleThreadedEffectEngine::state() const {
	return engineState;
}
const EffectRuntimeContext& SingleThreadedEffectEngine::context() const {
	return engineContext;
}

std::uint32_t SingleThreadedEffectEngine::particleCapacity() const {
	return engineParticleCapacity;
}
}
