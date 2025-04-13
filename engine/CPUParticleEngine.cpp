#include "CPUParticleEngine.h"
#include "../effect/ParticleRuntimePair.h"
#include <cmath>
#include <algorithm>
#include <unordered_set>

namespace pixelpart {
CPUParticleEngine::CPUParticleEngine(const Effect& effect, uint32_t particleCapacity) :
	ParticleEngine(), particleEffect(effect), engineParticleCapacity(particleCapacity), particleGenerator(effect, particleRuntimeInstances) {

}
#ifdef PIXELPART_MULTITHREADING
CPUParticleEngine::CPUParticleEngine(const Effect& effect, uint32_t particleCapacity, std::shared_ptr<ThreadPool> threadPl) :
	ParticleEngine(), particleEffect(effect), engineParticleCapacity(particleCapacity), particleGenerator(effect, particleRuntimeInstances), threadPool(threadPl) {

}
#endif

void CPUParticleEngine::step(float_t dt) {
	engineDeltaTime = dt;

	std::unordered_set<ParticleRuntimePair> neededRuntimeInstances;

	for(const ParticleEmitter* particleEmitter : particleEffect.sceneGraph().nodesWithType<ParticleEmitter>()) {
		for(id_t particleTypeId : particleEmitter->particleTypes()) {
			if(!particleRuntimeInstances.contains(particleEmitter->id(), particleTypeId)) {
				particleRuntimeInstances.add(particleEmitter->id(), particleTypeId, engineParticleCapacity);
			}

			neededRuntimeInstances.insert(ParticleRuntimePair(particleEmitter->id(), particleTypeId));
		}
	}

	particleRuntimeInstances.remove(std::remove_if(particleRuntimeInstances.begin(), particleRuntimeInstances.end(),
		[&neededRuntimeInstances](const ParticleRuntimeInstance& instance) {
			return neededRuntimeInstances.count(instance.key()) == 0u;
		}),
		particleRuntimeInstances.end());

	RuntimeContext rtContext = runtimeContext();

	particleGenerator.generate(rtContext);
	sizeModifier.prepare(particleEffect, rtContext);
	colorModifier.prepare(particleEffect, rtContext);
	accelerationModifier.prepare(particleEffect, rtContext);
	forceModifier.prepare(particleEffect, rtContext);
	collisionModifier.prepare(particleEffect, rtContext);
	motionPathModifier.prepare(particleEffect, rtContext);
	rotationModifier.prepare(particleEffect, rtContext);
	integrationModifier.prepare(particleEffect, rtContext);
	lifeModifier.prepare(particleEffect, rtContext);

	totalActiveThreadCount = 1u;

	for(ParticleRuntimeInstance& runtimeInstance : particleRuntimeInstances) {
		const ParticleType& particleType = particleEffect.particleTypes().at(runtimeInstance.typeId());
		const ParticleEmitter& particleEmitter = particleEffect.sceneGraph().at<ParticleEmitter>(runtimeInstance.emitterId());

		runtimeInstance.particles().removeDead();

		uint32_t activeThreadCount = 1u;

#ifdef PIXELPART_MULTITHREADING
		uint32_t availableThreadCount = threadPool != nullptr ? static_cast<uint32_t>(threadPool->threadCount()) : 1u;
		activeThreadCount = std::min(std::max(runtimeInstance.particles().count() / workPerThread, 1u), availableThreadCount);

		if(activeThreadCount > 1u) {
			uint32_t particleCountInThisThread = runtimeInstance.particles().count() / activeThreadCount;
			uint32_t threadsWithLargerLoadCount = runtimeInstance.particles().count() % activeThreadCount;
			uint32_t workgroupIndex = 0u;

			for(uint32_t threadIndex = 0u; threadIndex < activeThreadCount; threadIndex++) {
				uint32_t workgroupSize = (threadIndex < activeThreadCount - threadsWithLargerLoadCount)
					? particleCountInThisThread
					: particleCountInThisThread + 1u;

				threadPool->enqueue(threadIndex, &CPUParticleEngine::stepParticles, this,
					particleEmitter, particleType,
					runtimeInstance.particles().writePtr(workgroupIndex), workgroupSize,
					rtContext);

				workgroupIndex += workgroupSize;
			}

			for(uint32_t threadIndex = 0u; threadIndex < activeThreadCount; threadIndex++) {
				threadPool->wait(threadIndex);
			}
		}
#endif

		if(activeThreadCount <= 1u) {
			stepParticles(particleEmitter, particleType,
				runtimeInstance.particles().writePtr(), runtimeInstance.particles().count(),
				rtContext);
		}

		if(!particleEffect.is3d()) {
			ParticleCollection::WritePtr particles = runtimeInstance.particles().writePtr();
			for(uint32_t p = 0u; p < runtimeInstance.particles().count(); p++) {
				particles.rotation[p].y = 0.0;
				particles.rotation[p].z = 0.0;
				particles.force[p].z = 0.0;
				particles.velocity[p].z = 0.0;
				particles.position[p].z = 0.0;
				particles.globalPosition[p].z = 0.0;
			}
		}

		totalActiveThreadCount = std::max(totalActiveThreadCount, activeThreadCount);
	}

	engineTime += dt;
}
void CPUParticleEngine::restart(bool reset) {
	engineTime = 0.0;
	engineDeltaTime = 0.0;
	triggerActivationTimes.clear();

	if(reset) {
		particleGenerator.reset();
	}
}

float_t CPUParticleEngine::currentTime() const {
	return engineTime;
}
RuntimeContext CPUParticleEngine::runtimeContext() const {
	return RuntimeContext(engineTime, engineDeltaTime, triggerActivationTimes);
}

void CPUParticleEngine::seed(uint32_t seed) {
	particleGenerator.seed(seed);
}

void CPUParticleEngine::activateTrigger(id_t triggerId) {
	if(particleEffect.triggers().count(triggerId) == 0 ||
		triggerActivationTimes.count(triggerId) != 0) {
		return;
	}

	triggerActivationTimes[triggerId] = engineTime;
}

void CPUParticleEngine::spawnParticles(id_t particleEmitterId, uint32_t count, float_t time) {
	for(ParticleRuntimeInstance& runtimeInstance : particleRuntimeInstances) {
		if(runtimeInstance.emitterId() != particleEmitterId) {
			continue;
		}

		const ParticleEmitter& particleEmitter = particleEffect.sceneGraph().at<ParticleEmitter>(runtimeInstance.emitterId());
		if(!particleEmitter.primary()) {
			continue;
		}

		particleGenerator.generate(particleEmitterId, runtimeInstance.typeId(), count, time);
	}
}
void CPUParticleEngine::spawnParticles(id_t particleEmitterId, id_t particleTypeId, uint32_t count, float_t time) {
	ParticleRuntimeInstance* runtimeInstance = particleRuntimeInstances.find(particleEmitterId, particleTypeId);
	if(!runtimeInstance) {
		return;
	}

	const ParticleEmitter& particleEmitter = particleEffect.sceneGraph().at<ParticleEmitter>(runtimeInstance->emitterId());
	if(!particleEmitter.primary()) {
		return;
	}

	particleGenerator.generate(particleEmitterId, particleTypeId, count, time);
}

uint32_t CPUParticleEngine::particleCount() const {
	uint32_t count = 0u;
	for(const ParticleRuntimeInstance& runtimeInstance : particleRuntimeInstances) {
		count += runtimeInstance.particles().count();
	}

	return count;
}
uint32_t CPUParticleEngine::particleCount(id_t particleEmitterId, id_t particleTypeId) const {
	const ParticleRuntimeInstance* runtimeInstance = particleRuntimeInstances.find(particleEmitterId, particleTypeId);
	if(!runtimeInstance) {
		return 0u;
	}

	return runtimeInstance->particles().count();
}

const ParticleCollection* CPUParticleEngine::particles(id_t particleEmitterId, id_t particleTypeId) const {
	const ParticleRuntimeInstance* runtimeInstance = particleRuntimeInstances.find(particleEmitterId, particleTypeId);
	if(!runtimeInstance) {
		return nullptr;
	}

	return &runtimeInstance->particles();
}

const Effect& CPUParticleEngine::effect() const {
	return particleEffect;
}

uint32_t CPUParticleEngine::particleCapacity() const {
	return engineParticleCapacity;
}

void CPUParticleEngine::particleCountPerThread(uint32_t count) {
	workPerThread = std::max(count, 1u);
}
uint32_t CPUParticleEngine::particleCountPerThread() const {
	return workPerThread;
}
uint32_t CPUParticleEngine::activeThreadCount() const {
	return totalActiveThreadCount;
}

void CPUParticleEngine::stepParticles(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, uint32_t particleCount,
	const RuntimeContext& rtContext) const {
	sizeModifier.run(particleEffect.sceneGraph(), particleEmitter, particleType, particles, particleCount, rtContext);
	colorModifier.run(particleEffect.sceneGraph(), particleEmitter, particleType, particles, particleCount, rtContext);
	accelerationModifier.run(particleEffect.sceneGraph(), particleEmitter, particleType, particles, particleCount, rtContext);
	forceModifier.run(particleEffect.sceneGraph(), particleEmitter, particleType, particles, particleCount, rtContext);
	collisionModifier.run(particleEffect.sceneGraph(), particleEmitter, particleType, particles, particleCount, rtContext);
	motionPathModifier.run(particleEffect.sceneGraph(), particleEmitter, particleType, particles, particleCount, rtContext);
	rotationModifier.run(particleEffect.sceneGraph(), particleEmitter, particleType, particles, particleCount, rtContext);
	integrationModifier.run(particleEffect.sceneGraph(), particleEmitter, particleType, particles, particleCount, rtContext);
	lifeModifier.run(particleEffect.sceneGraph(), particleEmitter, particleType, particles, particleCount, rtContext);
}
}