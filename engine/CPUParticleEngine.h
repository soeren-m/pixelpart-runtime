#pragma once

#include "../common/Types.h"
#include "../common/Id.h"
#include "ParticleEngine.h"
#include "ParticleRuntimeInstance.h"
#include "ParticleRuntimeInstanceCollection.h"
#include "ParticleGenerator.h"
#include "SizeModifier.h"
#include "ColorModifier.h"
#include "AccelerationModifier.h"
#include "ForceModifier.h"
#include "CollisionModifier.h"
#include "MotionPathModifier.h"
#include "RotationModifier.h"
#include "IntegrationModifier.h"
#include "LifeModifier.h"

#ifdef PIXELPART_MULTITHREADING
#include "../common/ThreadPool.h"
#include <memory>
#endif

namespace pixelpart {
class CPUParticleEngine : public ParticleEngine {
public:
	CPUParticleEngine(const Effect& effect, uint32_t particleCapacity);
#ifdef PIXELPART_MULTITHREADING
	CPUParticleEngine(const Effect& effect, uint32_t particleCapacity, std::shared_ptr<ThreadPool> threadPl);
#endif

	virtual void step(float_t dt) override;
	virtual void restart(bool reset) override;

	virtual float_t currentTime() const override;

	virtual void seed(uint32_t seed) override;

	virtual void spawnParticles(id_t particleEmitterId, uint32_t count, float_t time = 0.0) override;
	virtual void spawnParticles(id_t particleEmitterId, id_t particleTypeId, uint32_t count, float_t time = 0.0) override;

	virtual uint32_t particleCount() const override;
	virtual uint32_t particleCount(id_t particleEmitterId, id_t particleTypeId) const override;

	virtual const ParticleCollection* particles(id_t particleEmitterId, id_t particleTypeId) const override;

	virtual const Effect& effect() const override;

	uint32_t particleCapacity() const;

	void particleCountPerThread(uint32_t count);
	uint32_t particleCountPerThread() const;
	uint32_t activeThreadCount() const;

private:
	void stepParticles(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		ParticleCollection::WritePtr particles, uint32_t particleCount,
		float_t t, float_t dt) const;

	const Effect& particleEffect;

	ParticleRuntimeInstanceCollection particleRuntimeInstances;

	uint32_t engineParticleCapacity = 0u;
	float_t engineTime = 0.0;

	ParticleGenerator particleGenerator;

	SizeModifier sizeModifier;
	ColorModifier colorModifier;
	AccelerationModifier accelerationModifier;
	ForceModifier forceModifier;
	CollisionModifier collisionModifier;
	MotionPathModifier motionPathModifier;
	RotationModifier rotationModifier;
	IntegrationModifier integrationModifier;
	LifeModifier lifeModifier;

#ifdef PIXELPART_MULTITHREADING
	std::shared_ptr<ThreadPool> threadPool;
#endif
	uint32_t totalActiveThreadCount = 0u;
	uint32_t workPerThread = 128u;
};
}