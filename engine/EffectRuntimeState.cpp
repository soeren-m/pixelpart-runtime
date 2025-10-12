#include "EffectRuntimeState.h"
#include "../effect/ParticleEmissionPair.h"

namespace pixelpart {
ParticleCollectionMap& EffectRuntimeState::particleCollections() {
	return stateParticleCollections;
}
const ParticleCollectionMap& EffectRuntimeState::particleCollections() const {
	return stateParticleCollections;
}
ParticleCollection* EffectRuntimeState::particleCollection(id_t particleEmitterId, id_t particleTypeId) {
	auto it = stateParticleCollections.find(ParticleEmissionPair(particleEmitterId, particleTypeId));
	if(it == stateParticleCollections.end()) {
		return nullptr;
	}

	return &it->second;
}
const ParticleCollection* EffectRuntimeState::particleCollection(id_t particleEmitterId, id_t particleTypeId) const {
	auto it = stateParticleCollections.find(ParticleEmissionPair(particleEmitterId, particleTypeId));
	if(it == stateParticleCollections.end()) {
		return nullptr;
	}

	return &it->second;
}

ParticleEmissionStateMap& EffectRuntimeState::particleEmissionStates() {
	return stateParticleEmissionStates;
}
const ParticleEmissionStateMap& EffectRuntimeState::particleEmissionStates() const {
	return stateParticleEmissionStates;
}
ParticleEmissionState* EffectRuntimeState::particleEmissionState(id_t particleEmitterId, id_t particleTypeId) {
	auto it = stateParticleEmissionStates.find(ParticleEmissionPair(particleEmitterId, particleTypeId));
	if(it == stateParticleEmissionStates.end()) {
		return nullptr;
	}

	return &it->second;
}
const ParticleEmissionState* EffectRuntimeState::particleEmissionState(id_t particleEmitterId, id_t particleTypeId) const {
	auto it = stateParticleEmissionStates.find(ParticleEmissionPair(particleEmitterId, particleTypeId));
	if(it == stateParticleEmissionStates.end()) {
		return nullptr;
	}

	return &it->second;
}

std::uint32_t& EffectRuntimeState::particleIdCounter() {
	return stateParticleIdCounter;
}
std::uint32_t EffectRuntimeState::particleIdCounter() const {
	return stateParticleIdCounter;
}

std::mt19937& EffectRuntimeState::rng() {
	return stateRng;
}
std::mt19937 EffectRuntimeState::rng() const {
	return stateRng;
}
}
