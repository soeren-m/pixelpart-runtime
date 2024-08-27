#pragma once

#include "../json/json.hpp"
#include <cstdint>

namespace pixelpart {
enum class LightingMode : uint32_t {
	unlit = 0,
	lit = 1
};

NLOHMANN_JSON_SERIALIZE_ENUM(LightingMode, {
	{ LightingMode::unlit, "unlit" },
	{ LightingMode::lit, "lit" }
})
}