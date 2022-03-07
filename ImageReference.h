#pragma once

#include "Types.h"
#include "TextureOrigin.h"

namespace pixelpart {
struct ImageReference {
	std::string id;
	std::string path;
	TextureOrigin textureOrigin = TextureOrigin::top_left;
	vec2d texturePivot = vec2d(0.0);
	uint32_t framesRow = 1;
	uint32_t framesColumn = 1;
	uint32_t frame = 0;
	bool randomFrame = false;
};

void to_json(nlohmann::ordered_json& j, const ImageReference& img);
void from_json(const nlohmann::ordered_json& j, ImageReference& img);
}