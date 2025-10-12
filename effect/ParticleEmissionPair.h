#pragma once

#include "../common/Types.h"
#include "../common/Id.h"
#include "../common/HashCombine.h"

namespace pixelpart {
struct ParticleEmissionPair {
	ParticleEmissionPair() = default;
	ParticleEmissionPair(id_t emitter, id_t type);

	id_t emitterId;
	id_t typeId;
};

bool operator==(const ParticleEmissionPair& lhs, const ParticleEmissionPair& rhs);
}

template <>
struct std::hash<pixelpart::ParticleEmissionPair> {
	std::size_t operator()(const pixelpart::ParticleEmissionPair& pair) const {
		std::size_t seed = 0;
		pixelpart::combineHash(seed, pair.emitterId);
		pixelpart::combineHash(seed, pair.typeId);

		return seed;
	}
};
