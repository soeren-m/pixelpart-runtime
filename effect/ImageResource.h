#pragma once

#include "Resource.h"
#include <vector>

namespace pixelpart {
struct ImageResource : public Resource {
	uint32_t width = 0u;
	uint32_t height = 0u;
	uint32_t bpp = 0u;

	std::vector<unsigned char> data;
};

void to_json(nlohmann::ordered_json& j, const ImageResource& resource);
void from_json(const nlohmann::ordered_json& j, ImageResource& resource);
}