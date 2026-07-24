#pragma once

#include "ParticleCollectionMap.h"
#include "ParticleEmissionStateMap.h"
#include "../math/Pcg32.h"
#include "../types/Id.h"
#include <cstdint>

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

	pcg32& rng();

private:
	ParticleCollectionMap stateParticleCollections;
	ParticleEmissionStateMap stateParticleEmissionStates;
	std::uint32_t stateParticleIdCounter = 0;
	pcg32 stateRng;
};
}
