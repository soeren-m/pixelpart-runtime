#pragma once

#include "ParticleData.h"

namespace pixelpart {
class ParticleContainer {
public:
	ParticleContainer();
	ParticleContainer(uint32_t capacity);

	void reserve(uint32_t capacity);

	uint32_t spawn(uint32_t count);
	void kill(uint32_t i);
	void kill();

	ParticleData& get();
	const ParticleData& get() const;

	uint32_t getNumParticles() const;

private:
	ParticleData data;

	uint32_t particleCapacity = 0u;
	uint32_t numParticles = 0u;
};
}