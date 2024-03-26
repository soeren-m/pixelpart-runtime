#pragma once

#include "../common/Types.h"

namespace pixelpart {
struct ComputeOutputTarget {
	enum Type : uint32_t {
		result = 0,
		keyframe = 1
	};

	Type type = result;
	uint32_t index = 0u;
};

NLOHMANN_JSON_SERIALIZE_ENUM(ComputeOutputTarget::Type, {
	{ ComputeOutputTarget::result, "result" },
	{ ComputeOutputTarget::keyframe, "keyframe" }
})

void to_json(nlohmann::ordered_json& j, const ComputeOutputTarget& target);
void from_json(const nlohmann::ordered_json& j, ComputeOutputTarget& target);
}