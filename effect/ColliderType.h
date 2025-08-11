#pragma once

#include "../json/json.hpp"
#include <cstdint>

namespace pixelpart {
enum class ColliderType : std::uint32_t {
	line = 0,
	plane = 1
};

NLOHMANN_JSON_SERIALIZE_ENUM(ColliderType, {
	{ ColliderType::line, "line" },
	{ ColliderType::plane, "plane" }
})
}
