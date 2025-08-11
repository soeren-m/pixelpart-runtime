#pragma once

#include "../common/Types.h"
#include "../common/Id.h"

namespace pixelpart {
struct ParticleRuntimeId {
	ParticleRuntimeId() = default;
	ParticleRuntimeId(id_t emitter, id_t type);

	id_t emitterId;
	id_t typeId;
};

bool operator==(const ParticleRuntimeId& lhs, const ParticleRuntimeId& rhs);
}

template <>
struct std::hash<pixelpart::ParticleRuntimeId> {
	std::size_t operator()(const pixelpart::ParticleRuntimeId& key) const {
		return
			std::hash<pixelpart::id_t>()(key.emitterId) ^
			std::hash<pixelpart::id_t>()(key.typeId);
	}
};
