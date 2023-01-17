#pragma once

#include "Types.h"

namespace pixelpart {
enum class BlendMode : uint32_t {
	normal = 0,
	additive = 1,
	subtractive = 2
};

NLOHMANN_JSON_SERIALIZE_ENUM(BlendMode, {
	{ BlendMode::normal, "normal" },
	{ BlendMode::additive, "additive" },
	{ BlendMode::subtractive, "subtractive" }
})
}