#pragma once

#include "../json/json.hpp"
#include <cstdint>

namespace pixelpart {
enum class BlendMode : std::uint32_t {
	off = 0,
	normal = 1,
	additive = 2,
	subtractive = 3
};

NLOHMANN_JSON_SERIALIZE_ENUM(BlendMode, {
	{ BlendMode::off, "off" },
	{ BlendMode::normal, "normal" },
	{ BlendMode::additive, "additive" },
	{ BlendMode::subtractive, "subtractive" }
})
}
