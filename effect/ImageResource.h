#pragma once

#include "../common/Types.h"
#include <vector>

namespace pixelpart {
struct ImageResource {
	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t bpp = 0;
	std::vector<unsigned char> data;
};

void to_json(nlohmann::ordered_json& j, const ImageResource& resource);
void from_json(const nlohmann::ordered_json& j, ImageResource& resource);
}