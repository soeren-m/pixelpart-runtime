#pragma once

#include "Types.h"

namespace pixelpart {
struct ImageAnimation {
	uint32_t frames = 1;
	floatd duration = 1.0;
	bool loop = false;
};

void to_json(nlohmann::ordered_json& j, const ImageAnimation& animation);
void from_json(const nlohmann::ordered_json& j, ImageAnimation& animation);
}