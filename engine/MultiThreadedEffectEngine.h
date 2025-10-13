#pragma once

#if defined(PIXELPART_RUNTIME_MULTITHREADING) || defined(__INTELLISENSE__)

#include "EffectEngine.h"
#include "ParticleGenerator.h"
#include "ParticleModifier.h"
#include "EffectRuntimeState.h"
#include "../common/ThreadPool.h"
#include "../effect/EffectRuntimeContext.h"
#include "../effect/Effect.h"
#include <cstdint>
#include <memory>

namespace pixelpart {
class MultiThreadedEffectEngine : public EffectEngine {
public:
	MultiThreadedEffectEngine(const Effect& effect,
		std::shared_ptr<ParticleGenerator> particleGenerator,
		std::shared_ptr<ParticleModifier> particleModifier,
		std::shared_ptr<ThreadPool> threadPool,
		std::uint32_t particleCapacity);
	MultiThreadedEffectEngine(const Effect& effect,
		std::shared_ptr<ParticleGenerator> particleGenerator,
		std::shared_ptr<ParticleModifier> particleModifier,
		std::shared_ptr<ThreadPool> threadPool,
		std::uint32_t particleCapacity,
		const EffectRuntimeState& initialState,
		const EffectRuntimeContext& initialContext);

	virtual void advance(float_t dt) override;
	virtual void restart() override;
	virtual void reset(const EffectRuntimeState& initialState, EffectRuntimeContext initialContext) override;
	virtual void reseed(std::uint32_t seed) override;

	virtual void generateParticles(std::uint32_t count, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext = EffectRuntimeContext()) override;
	virtual void clearParticles() override;

	virtual void activateTrigger(id_t triggerId) override;

	virtual const Effect& effect() const override;

	virtual const EffectRuntimeState& state() const override;
	virtual const EffectRuntimeContext& context() const override;

	std::uint32_t particleCapacity() const;

	void particleCountPerThread(std::uint32_t count);
	std::uint32_t particleCountPerThread() const;
	std::uint32_t activeThreadCount() const;

private:
	const Effect& engineEffect;
	std::shared_ptr<ParticleGenerator> engineParticleGenerator;
	std::shared_ptr<ParticleModifier> engineParticleModifier;
	std::shared_ptr<ThreadPool> engineThreadPool;

	EffectRuntimeState engineState;
	EffectRuntimeContext engineContext;

	std::uint32_t engineParticleCapacity = 0;

	std::uint32_t engineParticleCountPerThread = 128;
	std::uint32_t engineActiveThreadCount = 0;
};
}

#endif
