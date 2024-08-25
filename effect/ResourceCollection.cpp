#include "ResourceCollection.h"

namespace pixelpart {
ResourceCollection::ResourceCollection(
	const std::unordered_map<std::string, ImageResource>& images,
	const std::unordered_map<std::string, MeshResource>& meshes,
	const std::unordered_map<std::string, MaterialResource>& materials,
	const std::unordered_map<std::string, VectorFieldResource>& vectorFields) :
	imageResources(images), meshResources(meshes), materialResources(materials), vectorFieldResources(vectorFields) {

}

void to_json(nlohmann::ordered_json& j, const ResourceCollection& resources) {
	j = nlohmann::ordered_json{
		{ "images", resources.images() },
		{ "meshes", resources.meshes() },
		{ "materials", resources.materials() },
		{ "vector_fields", resources.vectorFields() }
	};
}
void from_json(const nlohmann::ordered_json& j, ResourceCollection& resources) {
	resources = ResourceCollection(
		j.value("images", std::unordered_map<std::string, ImageResource>()),
		j.value("meshes", std::unordered_map<std::string, MeshResource>()),
		j.value("materials", std::unordered_map<std::string, MaterialResource>()),
		j.value("vector_fields", std::unordered_map<std::string, VectorFieldResource>()));
}
}