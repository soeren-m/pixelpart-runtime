#pragma once

#include "../json/json.hpp"
#include <cstdint>

namespace pixelpart {
enum class AlignmentMode : std::uint32_t {
	none = 0,
	camera = 1,
	motion = 2,
	emission = 3,
	emitter = 4
};

NLOHMANN_JSON_SERIALIZE_ENUM(AlignmentMode, {
	{ AlignmentMode::none, "none" },
	{ AlignmentMode::camera, "camera" },
	{ AlignmentMode::motion, "motion" },
	{ AlignmentMode::emission, "emission" },
	{ AlignmentMode::emitter, "emitter" }
})
}
