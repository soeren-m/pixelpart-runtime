#ifdef PIXELPART_RUNTIME_MULTITHREADING

#include "MultiThreadedEffectEngine.h"
#include "../effect/ParticleRuntimeId.h"
#include <algorithm>

namespace pixelpart {
MultiThreadedEffectEngine::MultiThreadedEffectEngine(const Effect& effect, uint32_t particleCapacity, std::shared_ptr<ThreadPool> threadPl) :
	EffectEngine(), engineEffect(effect), engineParticleCapacity(particleCapacity), particleGenerator(effect, particleRuntimeInstances),
	threadPool(threadPl) {

}

void MultiThreadedEffectEngine::advance(float_t dt) {
	engineDeltaTime = dt;
	RuntimeContext rtContext = runtimeContext();

	particleRuntimeInstances.match(engineEffect, engineParticleCapacity);

	particleGenerator.generate(rtContext);
	particleModifierPipeline.prepare(engineEffect, rtContext);

	totalActiveThreadCount = 1;

	for(ParticleRuntimeInstance& runtimeInstance : particleRuntimeInstances) {
		const ParticleType& particleType = engineEffect.particleTypes().at(runtimeInstance.typeId());
		const ParticleEmitter& particleEmitter = engineEffect.sceneGraph().at<ParticleEmitter>(runtimeInstance.emitterId());

		runtimeInstance.particles().removeDead();

		uint32_t activeThreadCount = 1;

		uint32_t availableThreadCount = threadPool ? static_cast<uint32_t>(threadPool->threadCount()) : 1;
		activeThreadCount = std::min(std::max(runtimeInstance.particles().count() / workPerThread, 1u), availableThreadCount);

		if(activeThreadCount > 1) {
			uint32_t particleCountInThisThread = runtimeInstance.particles().count() / activeThreadCount;
			uint32_t threadsWithLargerLoadCount = runtimeInstance.particles().count() % activeThreadCount;
			uint32_t workgroupIndex = 0;

			for(uint32_t threadIndex = 0; threadIndex < activeThreadCount; threadIndex++) {
				uint32_t workgroupSize = (threadIndex < activeThreadCount - threadsWithLargerLoadCount)
					? particleCountInThisThread
					: particleCountInThisThread + 1;

				threadPool->enqueue(threadIndex, &ParticleModifierPipeline::run, &particleModifierPipeline,
					engineEffect.sceneGraph(), particleEmitter, particleType,
					runtimeInstance.particles().writePtr(workgroupIndex), workgroupSize,
					rtContext);

				workgroupIndex += workgroupSize;
			}

			for(uint32_t threadIndex = 0; threadIndex < activeThreadCount; threadIndex++) {
				threadPool->wait(threadIndex);
			}
		}
		else {
			particleModifierPipeline.run(
				engineEffect.sceneGraph(), particleEmitter, particleType,
				runtimeInstance.particles().writePtr(), runtimeInstance.particles().count(),
				rtContext);
		}

		totalActiveThreadCount = std::max(totalActiveThreadCount, activeThreadCount);
	}

	engineTime += dt;
}
void MultiThreadedEffectEngine::restart(bool reset) {
	engineTime = 0.0;
	engineDeltaTime = 0.0;
	triggerActivationTimes.clear();

	if(reset) {
		particleGenerator.reset();
	}
}

void MultiThreadedEffectEngine::seed(uint32_t seed) {
	particleGenerator.seed(seed);
}

void MultiThreadedEffectEngine::activateTrigger(id_t triggerId) {
	if(engineEffect.triggers().count(triggerId) == 0 ||
		triggerActivationTimes.count(triggerId) != 0) {
		return;
	}

	triggerActivationTimes[triggerId] = engineTime;
}

void MultiThreadedEffectEngine::spawnParticles(id_t particleEmitterId, uint32_t count, float_t time) {
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
void MultiThreadedEffectEngine::spawnParticles(id_t particleEmitterId, id_t particleTypeId, uint32_t count, float_t time) {
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

const ParticleCollection* MultiThreadedEffectEngine::particles(id_t particleEmitterId, id_t particleTypeId) const {
	const ParticleRuntimeInstance* runtimeInstance = particleRuntimeInstances.find(particleEmitterId, particleTypeId);
	if(!runtimeInstance) {
		return nullptr;
	}

	return &runtimeInstance->particles();
}

uint32_t MultiThreadedEffectEngine::particleCount(id_t particleEmitterId, id_t particleTypeId) const {
	const ParticleRuntimeInstance* runtimeInstance = particleRuntimeInstances.find(particleEmitterId, particleTypeId);
	if(!runtimeInstance) {
		return 0;
	}

	return runtimeInstance->particles().count();
}
uint32_t MultiThreadedEffectEngine::particleCount() const {
	uint32_t count = 0;
	for(const ParticleRuntimeInstance& runtimeInstance : particleRuntimeInstances) {
		count += runtimeInstance.particles().count();
	}

	return count;
}

RuntimeContext MultiThreadedEffectEngine::runtimeContext() const {
	return RuntimeContext(engineTime, engineDeltaTime, triggerActivationTimes);
}

const Effect& MultiThreadedEffectEngine::effect() const {
	return engineEffect;
}

uint32_t MultiThreadedEffectEngine::particleCapacity() const {
	return engineParticleCapacity;
}

void MultiThreadedEffectEngine::particleCountPerThread(uint32_t count) {
	workPerThread = std::max(count, 1u);
}
uint32_t MultiThreadedEffectEngine::particleCountPerThread() const {
	return workPerThread;
}

uint32_t MultiThreadedEffectEngine::activeThreadCount() const {
	return totalActiveThreadCount;
}
}

#endif