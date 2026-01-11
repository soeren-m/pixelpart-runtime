#pragma once

#include "../json/json.hpp"
#include <cstdint>

namespace pixelpart {
enum class BlendMode : std::uint32_t {
	off = 0,
	alpha = 1,
	additive = 2,
	subtractive = 3
};

NLOHMANN_JSON_SERIALIZE_ENUM(BlendMode, {
	{ BlendMode::off, "off" },
	{ BlendMode::alpha, "alpha" },
	{ BlendMode::alpha, "normal" },
	{ BlendMode::additive, "additive" },
	{ BlendMode::subtractive, "subtractive" }
})
}
