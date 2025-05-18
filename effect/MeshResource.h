#pragma once

#include "Resource.h"
#include "../glm/glm.hpp"
#include "../json/json.hpp"
#include <cstdint>
#include <vector>

namespace pixelpart {
class MeshResource : public Resource {
public:
	MeshResource() = default;
	MeshResource(const std::string& name);

	std::vector<std::uint32_t>& faces();
	const std::vector<std::uint32_t>& faces() const;

	std::vector<glm::vec3>& positions();
	const std::vector<glm::vec3>& positions() const;

	std::vector<glm::vec3>& normals();
	const std::vector<glm::vec3>& normals() const;

	std::vector<glm::vec2>& textureCoords();
	const std::vector<glm::vec2>& textureCoords() const;

private:
	std::vector<std::uint32_t> meshFaces;
	std::vector<glm::vec3> meshPositions;
	std::vector<glm::vec3> meshNormals;
	std::vector<glm::vec2> meshTextureCoords;
};

void to_json(nlohmann::ordered_json& j, const MeshResource& resource);
void from_json(const nlohmann::ordered_json& j, MeshResource& resource);
}