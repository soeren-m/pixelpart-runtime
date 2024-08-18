#pragma once

#include <cstdint>
#include "../json/json.hpp"

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