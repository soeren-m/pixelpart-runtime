#pragma once

#include <cstdint>
#include "../json/json.hpp"

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