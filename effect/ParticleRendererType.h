#pragma once

#include "../common/Types.h"

namespace pixelpart {
enum class ParticleRendererType : uint32_t {
	sprite = 0,
	trail = 1,
	mesh = 2
};

NLOHMANN_JSON_SERIALIZE_ENUM(ParticleRendererType, {
	{ ParticleRendererType::sprite, "sprite" },
	{ ParticleRendererType::trail, "trail" },
	{ ParticleRendererType::mesh, "mesh" }
})
}