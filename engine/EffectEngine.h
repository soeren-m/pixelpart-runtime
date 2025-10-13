#pragma once

#include "../common/Types.h"
#include "../common/Id.h"
#include "../effect/Effect.h"
#include "../effect/EffectRuntimeContext.h"
#include "EffectRuntimeState.h"
#include <cstdint>

namespace pixelpart {
class EffectEngine {
public:
	virtual ~EffectEngine() = default;

	virtual void advance(float_t dt) = 0;
	virtual void restart() = 0;
	virtual void reset(const EffectRuntimeState& initialState, EffectRuntimeContext initialContext) = 0;
	virtual void reseed(std::uint32_t seed) = 0;

	virtual void generateParticles(std::uint32_t count, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) = 0;
	virtual void clearParticles() = 0;

	virtual void activateTrigger(id_t triggerId) = 0;

	virtual const Effect& effect() const = 0;

	virtual const EffectRuntimeState& state() const = 0;
	virtual const EffectRuntimeContext& context() const = 0;
};
}
