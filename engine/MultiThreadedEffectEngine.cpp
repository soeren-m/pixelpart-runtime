#include "MultiThreadedEffectEngine.h"
#include "ParticleCollection.h"
#include "ParticleEmissionState.h"
#include "MapSync.h"
#include "../effect/ParticleEmissionPair.h"
#include <algorithm>

namespace pixelpart {
MultiThreadedEffectEngine::MultiThreadedEffectEngine(const Effect& effect,
	std::shared_ptr<ParticleGenerator> particleGenerator,
	std::shared_ptr<ParticleModifier> particleModifier,
	std::shared_ptr<ThreadPool> threadPool,
	std::uint32_t maxParticleCount) : EffectEngine(),
	engineEffect(effect),
	engineParticleGenerator(particleGenerator),
	engineParticleModifier(particleModifier),
	engineThreadPool(threadPool),
	engineMaxParticleCount(maxParticleCount) {

}
MultiThreadedEffectEngine::MultiThreadedEffectEngine(const Effect& effect,
	std::shared_ptr<ParticleGenerator> particleGenerator,
	std::shared_ptr<ParticleModifier> particleModifier,
	std::shared_ptr<ThreadPool> threadPool,
	std::uint32_t maxParticleCount,
	const EffectRuntimeState& initialState,
	const EffectRuntimeContext& initialContext) : EffectEngine(),
	engineEffect(effect),
	engineParticleGenerator(particleGenerator),
	engineParticleModifier(particleModifier),
	engineThreadPool(threadPool),
	engineMaxParticleCount(maxParticleCount),
	engineState(initialState),
	engineContext(initialContext) {

}

void MultiThreadedEffectEngine::advance(float_t dt) {
	dt = std::max(dt, 0.0);
	engineContext.deltaTime() = dt;

	syncMapToKeys(engineState.particleCollections(), engineEffect.particleEmissionPairs(), engineMaxParticleCount);
	syncMapToKeys(engineState.particleEmissionStates(), engineEffect.particleEmissionPairs());

	engineParticleGenerator->generate(engineState, &engineEffect, engineContext);

	engineParticleModifier->reset(&engineEffect, engineContext);

	engineActiveThreadCount = 1;

	for(auto& [emissionPair, particleCollection] : engineState.particleCollections()) {
		std::uint32_t availableThreadCount = engineThreadPool ? static_cast<std::uint32_t>(engineThreadPool->threadCount()) : 1;
		std::uint32_t activeThreadCount = std::min(std::max(particleCollection.count() / engineParticleCountPerThread, 1u), availableThreadCount);

		if(activeThreadCount > 1) {
			std::uint32_t particleCountInThisThread = particleCollection.count() / activeThreadCount;
			std::uint32_t threadsWithLargerLoadCount = particleCollection.count() % activeThreadCount;
			std::uint32_t workgroupIndex = 0;

			for(std::uint32_t threadIndex = 0; threadIndex < activeThreadCount; threadIndex++) {
				std::uint32_t workgroupSize = (threadIndex < activeThreadCount - threadsWithLargerLoadCount)
					? particleCountInThisThread
					: particleCountInThisThread + 1;

				engineThreadPool->enqueue(threadIndex, &ParticleModifier::apply, engineParticleModifier.get(),
					particleCollection.writePtr(workgroupIndex),
					workgroupSize,
					&engineEffect,
					emissionPair.emitterId, emissionPair.typeId,
					engineContext);

				workgroupIndex += workgroupSize;
			}

			for(std::uint32_t threadIndex = 0; threadIndex < activeThreadCount; threadIndex++) {
				engineThreadPool->wait(threadIndex);
			}
		}
		else {
			engineParticleModifier->apply(
				particleCollection.writePtr(),
				particleCollection.count(),
				&engineEffect,
				emissionPair.emitterId, emissionPair.typeId,
				engineContext);
		}

		engineActiveThreadCount = std::max(engineActiveThreadCount, activeThreadCount);
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
void MultiThreadedEffectEngine::restart() {
	engineContext.time() = 0.0;
	engineContext.deltaTime() = 0.0;
	engineContext.triggerActivationTimes().clear();
}
void MultiThreadedEffectEngine::reset(const EffectRuntimeState& initialState, EffectRuntimeContext initialContext) {
	engineState = initialState;
	engineContext = initialContext;
}
void MultiThreadedEffectEngine::reseed(std::uint32_t seed) {
	engineState.rng() = std::mt19937(seed);
}

void MultiThreadedEffectEngine::generateParticles(std::uint32_t count, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) {
	engineParticleGenerator->generate(engineState,
		count, &engineEffect, particleEmitterId, particleTypeId, runtimeContext);
}
void MultiThreadedEffectEngine::clearParticles() {
	engineParticleGenerator->clear(engineState);
}

void MultiThreadedEffectEngine::activateTrigger(id_t triggerId) {
	if(engineEffect.triggers().count(triggerId) == 0 ||
		engineContext.triggerActivationTimes().count(triggerId) != 0) {
		return;
	}

	engineContext.triggerActivationTimes()[triggerId] = engineContext.time() + engineContext.deltaTime() * 0.5;
}

void MultiThreadedEffectEngine::selectLod(std::uint32_t lod) {
	engineContext.lod() = lod;
}

const Effect& MultiThreadedEffectEngine::effect() const {
	return engineEffect;
}

const EffectRuntimeState& MultiThreadedEffectEngine::state() const {
	return engineState;
}
const EffectRuntimeContext& MultiThreadedEffectEngine::context() const {
	return engineContext;
}

void MultiThreadedEffectEngine::particleCountPerThread(std::uint32_t count) {
	engineParticleCountPerThread = std::max(count, 1u);
}
std::uint32_t MultiThreadedEffectEngine::particleCountPerThread() const {
	return engineParticleCountPerThread;
}
std::uint32_t MultiThreadedEffectEngine::activeThreadCount() const {
	return engineActiveThreadCount;
}
}
