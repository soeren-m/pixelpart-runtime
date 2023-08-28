#pragma once

#include "../effect/ImageResource.h"
#include <unordered_map>

namespace pixelpart {
struct ResourceDatabase {
	std::unordered_map<std::string, ImageResource> images;
};

void to_json(nlohmann::ordered_json& j, const ResourceDatabase& resources);
void from_json(const nlohmann::ordered_json& j, ResourceDatabase& resources);
}