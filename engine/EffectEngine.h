#pragma once

#include "../common/Types.h"
#include "../common/Id.h"
#include "../effect/Effect.h"
#include "../effect/RuntimeContext.h"
#include "ParticleCollection.h"
#include <cstdint>

namespace pixelpart {
class EffectEngine {
public:
	virtual ~EffectEngine() = default;

	virtual void advance(float_t dt) = 0;
	virtual void restart(bool reset) = 0;

	virtual void seed(std::uint32_t seed) = 0;

	virtual void activateTrigger(id_t triggerId) = 0;

	virtual void spawnParticles(id_t particleEmitterId, std::uint32_t count, float_t time = 0.0) = 0;
	virtual void spawnParticles(id_t particleEmitterId, id_t particleTypeId, std::uint32_t count, float_t time = 0.0) = 0;

	virtual const ParticleCollection* particles(id_t particleEmitterId, id_t particleTypeId) const = 0;

	virtual std::uint32_t particleCount(id_t particleEmitterId, id_t particleTypeId) const = 0;
	virtual std::uint32_t particleCount() const = 0;

	virtual RuntimeContext runtimeContext() const = 0;

	virtual const Effect& effect() const = 0;
};
}