#pragma once

#include "EffectEngine.h"
#include "ParticleGenerator.h"
#include "ParticleModifier.h"
#include "EffectRuntimeState.h"
#include "../effect/EffectRuntimeContext.h"
#include "../effect/Effect.h"
#include <cstdint>
#include <memory>

namespace pixelpart {
class SingleThreadedEffectEngine : public EffectEngine {
public:
	SingleThreadedEffectEngine(const Effect& effect,
		std::shared_ptr<ParticleGenerator> particleGenerator,
		std::shared_ptr<ParticleModifier> particleModifier,
		std::uint32_t particleCapacity);
	SingleThreadedEffectEngine(const Effect& effect,
		std::shared_ptr<ParticleGenerator> particleGenerator,
		std::shared_ptr<ParticleModifier> particleModifier,
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

private:
	const Effect& engineEffect;
	std::shared_ptr<ParticleGenerator> engineParticleGenerator;
	std::shared_ptr<ParticleModifier> engineParticleModifier;

	EffectRuntimeState engineState;
	EffectRuntimeContext engineContext;

	std::uint32_t engineParticleCapacity = 0;
};
}
