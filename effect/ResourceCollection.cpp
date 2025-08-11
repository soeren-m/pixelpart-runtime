#include "ResourceCollection.h"

namespace pixelpart {
ResourceCollection::ResourceCollection(
	const std::unordered_map<std::string, ImageResource>& images,
	const std::unordered_map<std::string, MeshResource>& meshes,
	const std::unordered_map<std::string, MaterialResource>& materials,
	const std::unordered_map<std::string, VectorFieldResource>& vectorFields) :
	imageResources(images), meshResources(meshes), materialResources(materials), vectorFieldResources(vectorFields) {

}

ImageResource& ResourceCollection::image(const std::string& name) {
	return imageResources.at(name);
}
const ImageResource& ResourceCollection::image(const std::string& name) const {
	return imageResources.at(name);
}
std::unordered_map<std::string, ImageResource>& ResourceCollection::images() {
	return imageResources;
}
const std::unordered_map<std::string, ImageResource>& ResourceCollection::images() const {
	return imageResources;
}

MeshResource& ResourceCollection::mesh(const std::string& name) {
	return meshResources.at(name);
}
const MeshResource& ResourceCollection::mesh(const std::string& name) const {
	return meshResources.at(name);
}
std::unordered_map<std::string, MeshResource>& ResourceCollection::meshes() {
	return meshResources;
}
const std::unordered_map<std::string, MeshResource>& ResourceCollection::meshes() const {
	return meshResources;
}

MaterialResource& ResourceCollection::material(const std::string& name) {
	return materialResources.at(name);
}
const MaterialResource& ResourceCollection::material(const std::string& name) const {
	return materialResources.at(name);
}
std::unordered_map<std::string, MaterialResource>& ResourceCollection::materials() {
	return materialResources;
}
const std::unordered_map<std::string, MaterialResource>& ResourceCollection::materials() const {
	return materialResources;
}

VectorFieldResource& ResourceCollection::vectorField(const std::string& name) {
	return vectorFieldResources.at(name);
}
const VectorFieldResource& ResourceCollection::vectorField(const std::string& name) const {
	return vectorFieldResources.at(name);
}
std::unordered_map<std::string, VectorFieldResource>& ResourceCollection::vectorFields() {
	return vectorFieldResources;
}
const std::unordered_map<std::string, VectorFieldResource>& ResourceCollection::vectorFields() const {
	return vectorFieldResources;
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
