#pragma once

#include "../common/Types.h"
#include "../common/Id.h"
#include "../effect/Effect.h"
#include "ParticleRuntimeInstance.h"
#include <vector>

namespace pixelpart {
class ParticleRuntimeInstanceCollection {
public:
	using iterator = typename std::vector<ParticleRuntimeInstance>::iterator;
	using const_iterator = typename std::vector<ParticleRuntimeInstance>::const_iterator;

	ParticleRuntimeInstanceCollection() = default;

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

	void match(const Effect& effect, std::uint32_t particleCapacity);

	ParticleRuntimeInstance& add(id_t particleEmitterId, id_t particleTypeId, std::uint32_t particleCapacity);
	void remove(id_t particleEmitterId, id_t particleTypeId);
	iterator remove(const_iterator position);
	iterator remove(const_iterator first, const_iterator last);

	ParticleRuntimeInstance& at(id_t particleEmitterId, id_t particleTypeId);
	const ParticleRuntimeInstance& at(id_t particleEmitterId, id_t particleTypeId) const;

	ParticleRuntimeInstance* find(id_t particleEmitterId, id_t particleTypeId);
	const ParticleRuntimeInstance* find(id_t particleEmitterId, id_t particleTypeId) const;

	bool contains(id_t particleEmitterId, id_t particleTypeId) const;

	std::size_t count() const;
	bool empty() const;

private:
	std::vector<ParticleRuntimeInstance> instances;
};
}