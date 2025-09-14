#include "ResourceCollection.h"
#include "../common/SortedJson.h"

namespace pixelpart {
ResourceCollection::ResourceCollection(
	const ResourceMap<ImageResource>& images,
	const ResourceMap<MeshResource>& meshes,
	const ResourceMap<MaterialResource>& materials,
	const ResourceMap<VectorFieldResource>& vectorFields) :
	imageResources(images), meshResources(meshes), materialResources(materials), vectorFieldResources(vectorFields) {

}

ImageResource& ResourceCollection::image(const std::string& name) {
	return imageResources.at(name);
}
const ImageResource& ResourceCollection::image(const std::string& name) const {
	return imageResources.at(name);
}
ResourceMap<ImageResource>& ResourceCollection::images() {
	return imageResources;
}
const ResourceMap<ImageResource>& ResourceCollection::images() const {
	return imageResources;
}

MeshResource& ResourceCollection::mesh(const std::string& name) {
	return meshResources.at(name);
}
const MeshResource& ResourceCollection::mesh(const std::string& name) const {
	return meshResources.at(name);
}
ResourceMap<MeshResource>& ResourceCollection::meshes() {
	return meshResources;
}
const ResourceMap<MeshResource>& ResourceCollection::meshes() const {
	return meshResources;
}

MaterialResource& ResourceCollection::material(const std::string& name) {
	return materialResources.at(name);
}
const MaterialResource& ResourceCollection::material(const std::string& name) const {
	return materialResources.at(name);
}
ResourceMap<MaterialResource>& ResourceCollection::materials() {
	return materialResources;
}
const ResourceMap<MaterialResource>& ResourceCollection::materials() const {
	return materialResources;
}

VectorFieldResource& ResourceCollection::vectorField(const std::string& name) {
	return vectorFieldResources.at(name);
}
const VectorFieldResource& ResourceCollection::vectorField(const std::string& name) const {
	return vectorFieldResources.at(name);
}
ResourceMap<VectorFieldResource>& ResourceCollection::vectorFields() {
	return vectorFieldResources;
}
const ResourceMap<VectorFieldResource>& ResourceCollection::vectorFields() const {
	return vectorFieldResources;
}

void to_json(nlohmann::ordered_json& j, const ResourceCollection& resources) {
	j = nlohmann::ordered_json{
		{ "images", toSortedJson(resources.images()) },
		{ "meshes", toSortedJson(resources.meshes()) },
		{ "materials", toSortedJson(resources.materials()) },
		{ "vector_fields", toSortedJson(resources.vectorFields()) }
	};
}
void from_json(const nlohmann::ordered_json& j, ResourceCollection& resources) {
	resources = ResourceCollection(
		j.value("images", ResourceMap<ImageResource>()),
		j.value("meshes", ResourceMap<MeshResource>()),
		j.value("materials", ResourceMap<MaterialResource>()),
		j.value("vector_fields", ResourceMap<VectorFieldResource>()));
}
}
