#pragma once

#include "Resource.h"
#include "../glm/glm.hpp"
#include "../json/json.hpp"
#include <cstdint>
#include <vector>

namespace pixelpart {
class MeshResource : public Resource {
public:

	// TODO: clear, ...

	const std::vector<uint32_t>& faces() const;
	const std::vector<glm::vec3>& positions() const;
	const std::vector<glm::vec3>& normals() const;
	const std::vector<glm::vec2>& textureCoords() const;

private:
	std::vector<uint32_t> faces;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> textureCoords;
};

void to_json(nlohmann::ordered_json& j, const MeshResource& resource);
void from_json(const nlohmann::ordered_json& j, MeshResource& resource);
}