#include "ParticleTypeCollection.h"

namespace pixelpart {
ParticleTypeCollection::ParticleTypeCollection(const std::vector<ParticleType>& types) {
	collectionParticleTypes = types;

	for(ParticleType& particleType : collectionParticleTypes) {
		if(particleType.id()) {
			continue;
		}

		id_t newId = 0;
		bool used = true;

		while(used) {
			newId++;
			used = false;

			for(const ParticleType& otherParticleType : collectionParticleTypes) {
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
	return collectionParticleTypes.begin();
}
ParticleTypeCollection::iterator ParticleTypeCollection::end() {
	return collectionParticleTypes.end();
}

ParticleTypeCollection::const_iterator ParticleTypeCollection::begin() const {
	return collectionParticleTypes.begin();
}
ParticleTypeCollection::const_iterator ParticleTypeCollection::end() const {
	return collectionParticleTypes.end();
}

ParticleType& ParticleTypeCollection::add() {
	id_t id = 0;
	while(contains(id)) {
		id++;
	}

	collectionParticleTypes.emplace_back(id);
	rebuildIndex();

	return collectionParticleTypes.back();
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

	collectionParticleTypes.emplace_back(particleType);

	ParticleType& insertedParticleType = collectionParticleTypes.back();
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
	std::optional<std::size_t> index = indexOf(id);
	if(!index) {
		return;
	}

	const ParticleType& particleType = collectionParticleTypes[index.value()];
	for(ParticleType& otherParticleType : collectionParticleTypes) {
		if(otherParticleType.parentId() == particleType.id()) {
			otherParticleType.parent(particleType.parentId());
		}
	}

	collectionParticleTypes.erase(collectionParticleTypes.begin() + index.value());
	rebuildIndex();
}
void ParticleTypeCollection::clear() {
	collectionParticleTypes.clear();
	rebuildIndex();
}

bool ParticleTypeCollection::contains(id_t id) const {
	return indexOf(id).has_value();
}

ParticleType& ParticleTypeCollection::at(id_t id) {
	return collectionParticleTypes.at(indexOf(id).value());
}
const ParticleType& ParticleTypeCollection::at(id_t id) const {
	return collectionParticleTypes.at(indexOf(id).value());
}

void ParticleTypeCollection::parent(id_t id, id_t parentId) {
	if(parentId == id) {
		return;
	}

	ParticleType& particleType = at(id);

	if(isDescendantOf(id, parentId)) {
		for(ParticleType& otherParticleType : collectionParticleTypes) {
			if(otherParticleType.parentId() == id) {
				otherParticleType.parent(particleType.parentId());
			}
		}
	}

	particleType.parent(parentId);
}
id_t ParticleTypeCollection::parentId(id_t id) const {
	return at(id).parentId();
}
std::vector<id_t> ParticleTypeCollection::childIds(id_t id) const {
	std::vector<id_t> result;
	for(const ParticleType& otherParticleType : collectionParticleTypes) {
		if(otherParticleType.parentId() == id) {
			result.push_back(otherParticleType.id());
		}
	}

	return result;
}
std::vector<id_t> ParticleTypeCollection::siblingIds(id_t id) const {
	std::vector<id_t> result;
	std::optional<std::size_t> index = indexOf(id);
	if(!index) {
		return result;
	}

	const ParticleType& particleType = collectionParticleTypes[index.value()];

	for(const ParticleType& otherParticleType : collectionParticleTypes) {
		if(otherParticleType.id() != id && otherParticleType.parentId() == particleType.parentId()) {
			result.push_back(otherParticleType.id());
		}
	}

	return result;
}
std::vector<id_t> ParticleTypeCollection::rootIds() const {
	std::vector<id_t> result;
	for(const ParticleType& particleType : collectionParticleTypes) {
		if(!particleType.parentId()) {
			result.push_back(particleType.id());
		}
	}

	return result;
}

std::size_t ParticleTypeCollection::count() const {
	return collectionParticleTypes.size();
}

void ParticleTypeCollection::rebuildIndex() {
	collectionIndexMap.clear();
	collectionIndexMap.reserve(collectionParticleTypes.size());

	for(std::size_t index = 0; index < collectionParticleTypes.size(); index++) {
		id_t id = collectionParticleTypes[index].particleTypeId;
		if(id.value() >= collectionIndexMap.size()) {
			collectionIndexMap.resize(id.value() + 1, id_t::nullValue);
		}

		collectionIndexMap[id.value()] = index;
	}
}
std::optional<std::size_t> ParticleTypeCollection::indexOf(id_t id) const {
	if(id.value() < collectionIndexMap.size() && collectionIndexMap[id.value()] != id_t::nullValue) {
		return collectionIndexMap[id.value()];
	}

	return std::nullopt;
}

bool ParticleTypeCollection::isDescendantOf(id_t parentId, id_t targetId) const {
	if(!parentId) {
		return false;
	}

	for(id_t childId : childIds(parentId)) {
		if(childId == targetId) {
			return true;
		}

		if(isDescendantOf(childId, targetId)) {
			return true;
		}
	}

	return false;
}

void to_json(nlohmann::ordered_json& j, const ParticleTypeCollection& collection) {
	j = nlohmann::ordered_json::array();
	for(const ParticleType& particleType : collection) {
		j.emplace_back(particleType);
	}
}
void from_json(const nlohmann::ordered_json& j, ParticleTypeCollection& collection) {
	collection = ParticleTypeCollection(j.get<std::vector<ParticleType>>());
}
}
