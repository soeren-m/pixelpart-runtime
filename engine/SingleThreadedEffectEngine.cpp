#include "SingleThreadedEffectEngine.h"
#include "ParticleCollection.h"
#include "ParticleEmissionState.h"
#include "../common/MapSync.h"
#include "../effect/ParticleEmissionPair.h"
#include <algorithm>

namespace pixelpart {
SingleThreadedEffectEngine::SingleThreadedEffectEngine(const Effect& effect,
	std::shared_ptr<ParticleGenerator> particleGenerator, std::shared_ptr<ParticleModifier> particleModifier,
	std::uint32_t maxParticleCount) : EffectEngine(),
	engineEffect(effect),
	engineParticleGenerator(particleGenerator),
	engineParticleModifier(particleModifier),
	engineMaxParticleCount(maxParticleCount) {

}
SingleThreadedEffectEngine::SingleThreadedEffectEngine(const Effect& effect,
	std::shared_ptr<ParticleGenerator> particleGenerator,
	std::shared_ptr<ParticleModifier> particleModifier,
	std::uint32_t maxParticleCount,
	const EffectRuntimeState& initialState,
	const EffectRuntimeContext& initialContext) : EffectEngine(),
	engineEffect(effect),
	engineParticleGenerator(particleGenerator),
	engineParticleModifier(particleModifier),
	engineState(initialState),
	engineContext(initialContext),
	engineMaxParticleCount(maxParticleCount) {

}

void SingleThreadedEffectEngine::advance(float_t dt) {
	dt = std::max(dt, 0.0);
	engineContext.deltaTime() = dt;

	syncMapToKeys(engineState.particleCollections(), engineEffect.particleEmissionPairs(), engineMaxParticleCount);
	syncMapToKeys(engineState.particleEmissionStates(), engineEffect.particleEmissionPairs());

	engineParticleGenerator->generate(engineState, &engineEffect, engineContext);

	engineParticleModifier->reset(&engineEffect, engineContext);

	for(auto& [emissionPair, particleCollection] : engineState.particleCollections()) {
		engineParticleModifier->apply(
			particleCollection.writePtr(),
			particleCollection.count(),
			&engineEffect,
			emissionPair.emitterId, emissionPair.typeId,
			engineContext);
	}

	engineContext.invokedEvents().clear();
	for(const auto& [eventId, event] : engineEffect.events()) {
		if(event.nodeId()) {
			const auto& node = engineEffect.sceneGraph().at(event.nodeId());

			if(node.activatedByTrigger(engineContext) &&
				engineContext.time() >= node.start() + event.time() &&
				engineContext.time() < node.start() + event.time() + dt) {
				engineContext.invokedEvents().push_back(eventId);
			}
		}
		else {
			if(engineContext.time() >= event.time() &&
				engineContext.time() < event.time() + dt) {
				engineContext.invokedEvents().push_back(eventId);
			}
		}
	}

	engineContext.time() += dt;
}
void SingleThreadedEffectEngine::restart() {
	engineContext.time() = 0.0;
	engineContext.deltaTime() = 0.0;
	engineContext.triggerActivationTimes().clear();
}
void SingleThreadedEffectEngine::reset(const EffectRuntimeState& initialState, EffectRuntimeContext initialContext) {
	engineState = initialState;
	engineContext = initialContext;
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
}
