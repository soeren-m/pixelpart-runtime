#pragma once

#include "ResourceMap.h"
#include "ImageResource.h"
#include "MeshResource.h"
#include "MaterialResource.h"
#include "VectorFieldResource.h"
#include "../json/json.hpp"
#include <string>

namespace pixelpart {
class ResourceCollection {
public:
	ResourceCollection() = default;
	ResourceCollection(
		const ResourceMap<ImageResource>& images,
		const ResourceMap<MeshResource>& meshes,
		const ResourceMap<MaterialResource>& materials,
		const ResourceMap<VectorFieldResource>& vectorFields);

	ImageResource& image(const std::string& name);
	const ImageResource& image(const std::string& name) const;
	ResourceMap<ImageResource>& images();
	const ResourceMap<ImageResource>& images() const;

	MeshResource& mesh(const std::string& name);
	const MeshResource& mesh(const std::string& name) const;
	ResourceMap<MeshResource>& meshes();
	const ResourceMap<MeshResource>& meshes() const;

	MaterialResource& material(const std::string& name);
	const MaterialResource& material(const std::string& name) const;
	ResourceMap<MaterialResource>& materials();
	const ResourceMap<MaterialResource>& materials() const;

	VectorFieldResource& vectorField(const std::string& name);
	const VectorFieldResource& vectorField(const std::string& name) const;
	ResourceMap<VectorFieldResource>& vectorFields();
	const ResourceMap<VectorFieldResource>& vectorFields() const;

private:
	ResourceMap<ImageResource> imageResources;
	ResourceMap<MeshResource> meshResources;
	ResourceMap<MaterialResource> materialResources;
	ResourceMap<VectorFieldResource> vectorFieldResources;
};

void to_json(nlohmann::ordered_json& j, const ResourceCollection& resources);
void from_json(const nlohmann::ordered_json& j, ResourceCollection& resources);
}
