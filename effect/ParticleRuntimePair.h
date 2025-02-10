#pragma once

#include "../common/Types.h"
#include "../common/Id.h"

namespace pixelpart {
struct ParticleRuntimePair {
	ParticleRuntimePair() = default;
	ParticleRuntimePair(id_t emitter, id_t type);

	id_t emitterId;
	id_t typeId;
};

bool operator==(const ParticleRuntimePair& lhs, const ParticleRuntimePair& rhs);
}

template <>
struct std::hash<pixelpart::ParticleRuntimePair> {
	std::size_t operator()(const pixelpart::ParticleRuntimePair& key) const {
		return
			std::hash<pixelpart::id_t>()(key.emitterId) ^
			std::hash<pixelpart::id_t>()(key.typeId);
	}
};