#pragma once

#include "ParticleType.h"
#include "../types/Types.h"
#include "../types/Id.h"
#include <optional>
#include <vector>

namespace pixelpart {
class ParticleTypeCollection {
public:
	using iterator = typename std::vector<ParticleType>::iterator;
	using const_iterator = typename std::vector<ParticleType>::const_iterator;

	ParticleTypeCollection() = default;
	ParticleTypeCollection(const std::vector<ParticleType>& types);

	iterator begin();
	iterator end();

	const_iterator begin() const;
	const_iterator end() const;

	ParticleType& add();
	ParticleType& add(const ParticleType& particleType, id_t baseId = id_t(0));
	ParticleType& duplicate(id_t id);
	void remove(id_t id);
	void clear();

	bool contains(id_t id) const;

	ParticleType& at(id_t id);
	const ParticleType& at(id_t id) const;

	void parent(id_t id, id_t parentId);
	id_t parentId(id_t id) const;
	std::vector<id_t> childIds(id_t id) const;
	std::vector<id_t> siblingIds(id_t id) const;
	std::vector<id_t> rootIds() const;

	std::size_t count() const;

private:
	void rebuildIndex();
	std::optional<std::size_t> indexOf(id_t id) const;

	bool isDescendantOf(id_t parentId, id_t targetId) const;

	std::vector<ParticleType> collectionParticleTypes;
	std::vector<std::size_t> collectionIndexMap;
};

void to_json(nlohmann::ordered_json& j, const ParticleTypeCollection& collection);
void from_json(const nlohmann::ordered_json& j, ParticleTypeCollection& collection);
}
