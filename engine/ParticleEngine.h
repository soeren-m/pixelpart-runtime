#pragma once

#include "ParticleData.h"
#include "../effect/Effect.h"

namespace pixelpart {
class ParticleEngine {
public:
	ParticleEngine(const Effect& fx);

	virtual void step(float_t dt) = 0;
	virtual void restart(bool reset) = 0;

	virtual float_t getTime() const = 0;

	virtual void applySeed(uint32_t seed) = 0;
	virtual void resetSeed() = 0;

	virtual void spawnParticles(id_t particleTypeId, uint32_t count) = 0;

	virtual uint32_t getNumParticles() const = 0;
	virtual uint32_t getNumParticles(uint32_t particleTypeIndex) const = 0;
	virtual const ParticleData& getParticles(uint32_t particleTypeIndex) const = 0;

	const Effect& getEffect() const;

protected:
	const Effect& effect;
};
}