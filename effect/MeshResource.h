#pragma once

#include "Resource.h"
#include <vector>

namespace pixelpart {
struct MeshResource : public Resource {
	std::vector<uint32_t> faces;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> textureCoords;
};

void to_json(nlohmann::ordered_json& j, const MeshResource& resource);
void from_json(const nlohmann::ordered_json& j, MeshResource& resource);
}