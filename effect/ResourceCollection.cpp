#include "ResourceCollection.h"
#include "../common/Json.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const ResourceCollection& resources) {
	j = nlohmann::ordered_json{
		{ "images", resources.images },
		{ "meshes", resources.meshes },
		{ "materials", resources.materials },
		{ "vector_fields", resources.vectorFields }
	};
}
void from_json(const nlohmann::ordered_json& j, ResourceCollection& resources) {
	resources = ResourceCollection();

	fromJson(resources.images, j, "images");
	fromJson(resources.meshes, j, "meshes");
	fromJson(resources.materials, j, "materials");
	fromJson(resources.vectorFields, j, "vector_fields");
}
}