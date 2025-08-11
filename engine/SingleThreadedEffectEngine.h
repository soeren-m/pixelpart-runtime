#pragma once

#include "EffectEngine.h"
#include "ParticleRuntimeInstance.h"
#include "ParticleRuntimeInstanceCollection.h"
#include "ParticleGenerator.h"
#include "ParticleModifierPipeline.h"

namespace pixelpart {
class SingleThreadedEffectEngine : public EffectEngine {
public:
	SingleThreadedEffectEngine(const Effect& effect, std::uint32_t particleCapacity);

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

private:
	const Effect& engineEffect;

	ParticleRuntimeInstanceCollection particleRuntimeInstances;
	std::uint32_t engineParticleCapacity = 0;

	float_t engineTime = 0.0;
	float_t engineDeltaTime = 0.0;
	RuntimeContext::TriggerActivationTimeMap triggerActivationTimes;

	ParticleGenerator particleGenerator;
	ParticleModifierPipeline particleModifierPipeline;
};
}
