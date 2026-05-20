#pragma once

#include "../json/json.hpp"
#include <cstdint>

namespace pixelpart {
enum class ParticleCoordinateSystem : std::uint32_t {
	global = 0,
	local = 1
};

NLOHMANN_JSON_SERIALIZE_ENUM(ParticleCoordinateSystem, {
	{ ParticleCoordinateSystem::global, "global" },
	{ ParticleCoordinateSystem::local, "local" }
})
}
