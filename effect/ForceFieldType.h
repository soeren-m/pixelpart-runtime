#pragma once

#include "../json/json.hpp"
#include <cstdint>

namespace pixelpart {
enum class ForceFieldType : uint32_t {
	attraction = 0,
	acceleration = 1,
	vector = 2,
	noise = 3,
	drag = 4
};

NLOHMANN_JSON_SERIALIZE_ENUM(ForceFieldType, {
	{ ForceFieldType::attraction, "attraction" },
	{ ForceFieldType::acceleration, "acceleration" },
	{ ForceFieldType::vector, "vector" },
	{ ForceFieldType::noise, "noise" },
	{ ForceFieldType::drag, "drag" }
})
}