#pragma once

#include "ParticleCollectionMap.h"
#include "ParticleEmissionStateMap.h"
#include "../common/Id.h"
#include <cstdint>
#include <random>

namespace pixelpart {
class EffectRuntimeState {
public:
	EffectRuntimeState() = default;

	ParticleCollectionMap& particleCollections();
	const ParticleCollectionMap& particleCollections() const;
	ParticleCollection* particleCollection(id_t particleEmitterId, id_t particleTypeId);
	const ParticleCollection* particleCollection(id_t particleEmitterId, id_t particleTypeId) const;

	ParticleEmissionStateMap& particleEmissionStates();
	const ParticleEmissionStateMap& particleEmissionStates() const;
	ParticleEmissionState* particleEmissionState(id_t particleEmitterId, id_t particleTypeId);
	const ParticleEmissionState* particleEmissionState(id_t particleEmitterId, id_t particleTypeId) const;

	std::uint32_t& particleIdCounter();
	std::uint32_t particleIdCounter() const;

	std::mt19937& rng();
	std::mt19937 rng() const;

private:
	ParticleCollectionMap stateParticleCollections;
	ParticleEmissionStateMap stateParticleEmissionStates;
	std::uint32_t stateParticleIdCounter = 0;
	std::mt19937 stateRng;
};
}
