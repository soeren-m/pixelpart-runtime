#pragma once

#include "../json/json.hpp"
#include <cstdint>

namespace pixelpart {
enum class ColorSpace : std::uint32_t {
	linear = 0,
	srgb = 1
};

NLOHMANN_JSON_SERIALIZE_ENUM(ColorSpace, {
	{ ColorSpace::linear, "linear" },
	{ ColorSpace::srgb, "srgb" }
})
}
