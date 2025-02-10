#pragma once

#include "../json/json.hpp"
#include <cstdint>

namespace pixelpart {
enum class RotationMode : uint32_t {
	angle = 0,
	velocity = 1
};

NLOHMANN_JSON_SERIALIZE_ENUM(RotationMode, {
	{ RotationMode::angle, "angle" },
	{ RotationMode::velocity, "velocity" }
})
}