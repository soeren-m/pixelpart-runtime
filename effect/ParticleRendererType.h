#pragma once

#include "../json/json.hpp"
#include <cstdint>

namespace pixelpart {
enum class ParticleRendererType : std::uint32_t {
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
