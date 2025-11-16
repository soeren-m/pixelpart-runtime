#include "ParticleTypeCollection.h"

namespace pixelpart {
ParticleTypeCollection::ParticleTypeCollection(const std::vector<ParticleType>& types) {
	particleTypes = types;

	for(ParticleType& particleType : particleTypes) {
		if(particleType.id()) {
			continue;
		}

		id_t newId = 0;
		bool used = true;

		while(used) {
			newId++;
			used = false;

			for(const ParticleType& otherParticleType : particleTypes) {
				if(otherParticleType.particleTypeId == newId) {
					used = true;
					break;
				}
			}
		}

		particleType.particleTypeId = newId;
	}

	rebuildIndex();
}

ParticleTypeCollection::iterator ParticleTypeCollection::begin() {
	return particleTypes.begin();
}
ParticleTypeCollection::iterator ParticleTypeCollection::end() {
	return particleTypes.end();
}
ParticleTypeCollection::const_iterator ParticleTypeCollection::begin() const {
	return particleTypes.begin();
}
ParticleTypeCollection::const_iterator ParticleTypeCollection::end() const {
	return particleTypes.end();
}

ParticleType& ParticleTypeCollection::add() {
	id_t id = 0;
	while(contains(id)) {
		id++;
	}

	particleTypes.emplace_back(id);
	rebuildIndex();

	return particleTypes.back();
}
ParticleType& ParticleTypeCollection::add(const ParticleType& particleType, id_t baseId) {
	id_t id = particleType.id()
		? id_t(baseId.value() + particleType.id().value())
		: baseId;
	id_t parentId = particleType.parentId()
		? id_t(baseId.value() + particleType.parentId().value())
		: id_t();

	while(contains(id)) {
		id++;
	}

	particleTypes.emplace_back(particleType);

	ParticleType& insertedParticleType = particleTypes.back();
	insertedParticleType.particleTypeId = id;
	insertedParticleType.parentParticleTypeId = parentId;
	rebuildIndex();

	return insertedParticleType;
}
ParticleType& ParticleTypeCollection::duplicate(id_t id) {
	const ParticleType& particleType = at(id);

	return add(particleType);
}
void ParticleTypeCollection::remove(id_t id) {
	removeIndex(indexOf(id));
}
void ParticleTypeCollection::removeIndex(std::uint32_t index) {
	if(index >= particleTypes.size()) {
		return;
	}

	const ParticleType& particleType = particleTypes[index];
	for(ParticleType& otherParticleType : particleTypes) {
		if(otherParticleType.parentId() == particleType.id()) {
			otherParticleType.parent(particleType.parentId());
		}
	}

	particleTypes.erase(particleTypes.begin() + index);
	rebuildIndex();
}
void ParticleTypeCollection::clear() {
	particleTypes.clear();
	rebuildIndex();
}

void ParticleTypeCollection::parent(id_t id, id_t parentId) {
	ParticleType& particleType = at(id);
	for(ParticleType& otherParticleType : particleTypes) {
		if(otherParticleType.parentId() == id) {
			otherParticleType.parent(particleType.parentId());
		}
	}

	particleType.parent(parentId);
}
id_t ParticleTypeCollection::parentId(id_t id) const {
	return at(id).parentId();
}
std::vector<id_t> ParticleTypeCollection::childIds(id_t id) const {
	std::vector<id_t> result;
	for(const ParticleType& otherParticleType : particleTypes) {
		if(otherParticleType.parentId() == id) {
			result.push_back(otherParticleType.id());
		}
	}

	return result;
}

std::uint32_t ParticleTypeCollection::count() const {
	return static_cast<std::uint32_t>(particleTypes.size());
}

std::uint32_t ParticleTypeCollection::indexOf(id_t id) const {
	return id.value() < indexMap.size() ? indexMap[id.value()] : id_t::nullValue;
}

bool ParticleTypeCollection::contains(id_t id) const {
	return indexOf(id) != id_t::nullValue;
}
bool ParticleTypeCollection::containsIndex(std::uint32_t index) const {
	return index < particleTypes.size();
}

ParticleType& ParticleTypeCollection::at(id_t id) {
	return particleTypes.at(indexOf(id));
}
const ParticleType& ParticleTypeCollection::at(id_t id) const {
	return particleTypes.at(indexOf(id));
}

ParticleType& ParticleTypeCollection::atIndex(std::uint32_t index) {
	return particleTypes.at(index);
}
const ParticleType& ParticleTypeCollection::atIndex(std::uint32_t index) const {
	return particleTypes.at(index);
}

void ParticleTypeCollection::rebuildIndex() {
	indexMap.clear();
	indexMap.reserve(particleTypes.size());

	for(std::uint32_t index = 0; index < particleTypes.size(); index++) {
		id_t id = particleTypes[index].particleTypeId;
		if(id.value() >= indexMap.size()) {
			indexMap.resize(id.value() + 1, id_t::nullValue);
		}

		indexMap[id.value()] = index;
	}
}

void to_json(nlohmann::ordered_json& j, const ParticleTypeCollection& collection) {
	j = nlohmann::ordered_json::array();
	for(std::uint32_t index = 0; index < collection.count(); index++) {
		j.emplace_back(collection.atIndex(index));
	}
}
void from_json(const nlohmann::ordered_json& j, ParticleTypeCollection& collection) {
	collection = ParticleTypeCollection(j.get<std::vector<ParticleType>>());
}
}
