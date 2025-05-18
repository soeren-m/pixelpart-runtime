#pragma once

#include "ParticleType.h"
#include "../common/Types.h"
#include "../common/Id.h"
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
	void removeIndex(std::uint32_t index);
	void clear();

	void parent(id_t id, id_t parentId);
	id_t parentId(id_t id) const;
	std::vector<id_t> childIds(id_t id) const;

	std::uint32_t count() const;

	std::uint32_t indexOf(id_t id) const;

	bool contains(id_t id) const;
	bool containsIndex(std::uint32_t index) const;

	ParticleType& at(id_t id);
	const ParticleType& at(id_t id) const;

	ParticleType& atIndex(std::uint32_t index);
	const ParticleType& atIndex(std::uint32_t index) const;

private:
	void rebuildIndex();

	std::vector<ParticleType> particleTypes;
	std::vector<std::uint32_t> indexMap;
};

void to_json(nlohmann::ordered_json& j, const ParticleTypeCollection& collection);
void from_json(const nlohmann::ordered_json& j, ParticleTypeCollection& collection);
}