#pragma once

#include "Resource.h"
#include "../math/Vector2.h"
#include "../math/Vector3.h"
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

	std::vector<math::vector3<float>>& positions();
	const std::vector<math::vector3<float>>& positions() const;

	std::vector<math::vector3<float>>& normals();
	const std::vector<math::vector3<float>>& normals() const;

	std::vector<math::vector2<float>>& textureCoords();
	const std::vector<math::vector2<float>>& textureCoords() const;

private:
	std::vector<std::uint32_t> meshFaces;
	std::vector<math::vector3<float>> meshPositions;
	std::vector<math::vector3<float>> meshNormals;
	std::vector<math::vector2<float>> meshTextureCoords;
};

void to_json(nlohmann::ordered_json& j, const MeshResource& resource);
void from_json(const nlohmann::ordered_json& j, MeshResource& resource);
}
