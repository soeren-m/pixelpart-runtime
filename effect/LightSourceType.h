#pragma once

#include "../json/json.hpp"
#include <cstdint>

namespace pixelpart {
enum class LightSourceType : std::uint32_t {
	directional = 0,
	point = 1,
	spot = 2
};

NLOHMANN_JSON_SERIALIZE_ENUM(LightSourceType, {
	{ LightSourceType::directional, "directional" },
	{ LightSourceType::point, "point" },
	{ LightSourceType::spot, "spot" }
})
}
