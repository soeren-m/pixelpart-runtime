#pragma once

#include "ImageResource.h"
#include "MeshResource.h"
#include "VectorFieldResource.h"
#include <unordered_map>

namespace pixelpart {
struct EffectResourceSet {
	std::unordered_map<std::string, ImageResource> images;
	std::unordered_map<std::string, MeshResource> meshes;
	std::unordered_map<std::string, VectorFieldResource> vectorFields;
};

void to_json(nlohmann::ordered_json& j, const EffectResourceSet& resources);
void from_json(const nlohmann::ordered_json& j, EffectResourceSet& resources);
}