#pragma once

#include "../common/Types.h"
#include "../common/Id.h"
#include "../effect/Effect.h"
#include "ParticleCollection.h"

namespace pixelpart {
class ParticleEngine {
public:
	ParticleEngine(const Effect& fx);

	virtual void step(float_t dt) = 0;
	virtual void restart(bool reset) = 0;

	virtual float_t currentTime() const = 0;

	virtual void applySeed(uint32_t seed) = 0;
	virtual void resetSeed() = 0;

	virtual void spawnParticles(id_t particleEmitterId, uint32_t count) = 0;
	virtual void spawnParticles(id_t particleEmitterId, id_t particleTypeId, uint32_t count) = 0;

	virtual uint32_t particleCount() const = 0;
	virtual uint32_t particleCount(id_t particleEmitterId, id_t particleTypeId) const = 0;

	virtual ParticleCollection::ReadPtr particles(id_t particleEmitterId, id_t particleTypeId) const = 0;

	virtual const Effect& effect() const = 0;
};
}