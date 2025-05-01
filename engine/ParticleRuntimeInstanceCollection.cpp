#include "ParticleRuntimeInstanceCollection.h"
#include <unordered_set>
#include <algorithm>
#include <stdexcept>

namespace pixelpart {
ParticleRuntimeInstanceCollection::iterator ParticleRuntimeInstanceCollection::begin() {
	return instances.begin();
}
ParticleRuntimeInstanceCollection::iterator ParticleRuntimeInstanceCollection::end() {
	return instances.end();
}
ParticleRuntimeInstanceCollection::const_iterator ParticleRuntimeInstanceCollection::begin() const {
	return instances.begin();
}
ParticleRuntimeInstanceCollection::const_iterator ParticleRuntimeInstanceCollection::end() const {
	return instances.end();
}

void ParticleRuntimeInstanceCollection::match(const Effect& effect, uint32_t particleCapacity) {
	std::unordered_set<ParticleRuntimeId> neededRuntimeInstances;

	for(const ParticleEmitter* particleEmitter : effect.sceneGraph().nodesWithType<ParticleEmitter>()) {
		for(id_t particleTypeId : particleEmitter->particleTypes()) {
			if(!contains(particleEmitter->id(), particleTypeId)) {
				add(particleEmitter->id(), particleTypeId, particleCapacity);
			}

			neededRuntimeInstances.insert(ParticleRuntimeId(particleEmitter->id(), particleTypeId));
		}
	}

	remove(std::remove_if(begin(), end(),
		[&neededRuntimeInstances](const ParticleRuntimeInstance& instance) {
			return neededRuntimeInstances.count(instance.id()) == 0;
		}),
		end());
}

ParticleRuntimeInstance& ParticleRuntimeInstanceCollection::add(id_t particleEmitterId, id_t particleTypeId, uint32_t particleCapacity) {
	instances.emplace_back(particleEmitterId, particleTypeId, particleCapacity);

	return instances.back();
}
void ParticleRuntimeInstanceCollection::remove(id_t particleEmitterId, id_t particleTypeId) {
	instances.erase(std::remove_if(instances.begin(), instances.end(),
		[particleEmitterId, particleTypeId](const ParticleRuntimeInstance& instance) {
			return instance.emitterId() == particleEmitterId && instance.typeId() == particleTypeId;
		}),
		instances.end());
}
ParticleRuntimeInstanceCollection::iterator ParticleRuntimeInstanceCollection::remove(const_iterator position) {
	return instances.erase(position);
}
ParticleRuntimeInstanceCollection::iterator ParticleRuntimeInstanceCollection::remove(const_iterator first, const_iterator last) {
	return instances.erase(first, last);
}

ParticleRuntimeInstance& ParticleRuntimeInstanceCollection::at(id_t particleEmitterId, id_t particleTypeId) {
	ParticleRuntimeInstance* instance = find(particleEmitterId, particleTypeId);
	if(!instance) {
		throw std::out_of_range("Particle runtime instance not found");
	}

	return *instance;
}
const ParticleRuntimeInstance& ParticleRuntimeInstanceCollection::at(id_t particleEmitterId, id_t particleTypeId) const {
	const ParticleRuntimeInstance* instance = find(particleEmitterId, particleTypeId);
	if(!instance) {
		throw std::out_of_range("Particle runtime instance not found");
	}

	return *instance;
}

ParticleRuntimeInstance* ParticleRuntimeInstanceCollection::find(id_t particleEmitterId, id_t particleTypeId) {
	for(ParticleRuntimeInstance& instance : instances) {
		if(instance.emitterId() == particleEmitterId && instance.typeId() == particleTypeId) {
			return &instance;
		}
	}

	return nullptr;
}
const ParticleRuntimeInstance* ParticleRuntimeInstanceCollection::find(id_t particleEmitterId, id_t particleTypeId) const {
	for(const ParticleRuntimeInstance& instance : instances) {
		if(instance.emitterId() == particleEmitterId && instance.typeId() == particleTypeId) {
			return &instance;
		}
	}

	return nullptr;
}

bool ParticleRuntimeInstanceCollection::contains(id_t particleEmitterId, id_t particleTypeId) const {
	for(const ParticleRuntimeInstance& instance : instances) {
		if(instance.emitterId() == particleEmitterId && instance.typeId() == particleTypeId) {
			return true;
		}
	}

	return false;
}

std::size_t ParticleRuntimeInstanceCollection::count() const {
	return instances.size();
}
bool ParticleRuntimeInstanceCollection::empty() const {
	return instances.empty();
}
}