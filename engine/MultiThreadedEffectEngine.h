#pragma once

#ifdef PIXELPART_RUNTIME_MULTITHREADING

#include "EffectEngine.h"
#include "ParticleRuntimeInstance.h"
#include "ParticleRuntimeInstanceCollection.h"
#include "ParticleGenerator.h"
#include "ParticleModifierPipeline.h"
#include "../common/ThreadPool.h"
#include <memory>

namespace pixelpart {
class MultiThreadedEffectEngine : public EffectEngine {
public:
	MultiThreadedEffectEngine(const Effect& effect, std::uint32_t particleCapacity, std::shared_ptr<ThreadPool> threadPl);

	virtual void advance(float_t dt) override;
	virtual void restart(bool reset) override;

	virtual void seed(std::uint32_t seed) override;

	virtual void activateTrigger(id_t triggerId) override;

	virtual void spawnParticles(id_t particleEmitterId, std::uint32_t count, float_t time = 0.0) override;
	virtual void spawnParticles(id_t particleEmitterId, id_t particleTypeId, std::uint32_t count, float_t time = 0.0) override;

	virtual const ParticleCollection* particles(id_t particleEmitterId, id_t particleTypeId) const override;

	virtual std::uint32_t particleCount(id_t particleEmitterId, id_t particleTypeId) const override;
	virtual std::uint32_t particleCount() const override;

	virtual RuntimeContext runtimeContext() const override;

	virtual const Effect& effect() const override;

	std::uint32_t particleCapacity() const;

	void particleCountPerThread(std::uint32_t count);
	std::uint32_t particleCountPerThread() const;

	std::uint32_t activeThreadCount() const;

private:
	const Effect& engineEffect;

	ParticleRuntimeInstanceCollection particleRuntimeInstances;
	std::uint32_t engineParticleCapacity = 0;

	float_t engineTime = 0.0;
	float_t engineDeltaTime = 0.0;
	RuntimeContext::TriggerActivationTimeMap triggerActivationTimes;

	ParticleGenerator particleGenerator;
	ParticleModifierPipeline particleModifierPipeline;

	std::shared_ptr<ThreadPool> threadPool;
	std::uint32_t workPerThread = 128;
	std::uint32_t totalActiveThreadCount = 0;
};
}

#endif