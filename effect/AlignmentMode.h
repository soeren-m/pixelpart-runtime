#pragma once

#include <cstdint>
#include "../json/json.hpp"

namespace pixelpart {
enum class AlignmentMode : uint32_t {
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