#pragma once

#include "ImageResource.h"
#include "MeshResource.h"
#include "MaterialResource.h"
#include "VectorFieldResource.h"
#include "../json/json.hpp"
#include <string>
#include <unordered_map>

namespace pixelpart {
class ResourceCollection {
public:
	ResourceCollection() = default;
	ResourceCollection(
		const std::unordered_map<std::string, ImageResource>& images,
		const std::unordered_map<std::string, MeshResource>& meshes,
		const std::unordered_map<std::string, MaterialResource>& materials,
		const std::unordered_map<std::string, VectorFieldResource>& vectorFields);

	ImageResource& image(const std::string& name);
	const ImageResource& image(const std::string& name) const;

	const std::unordered_map<std::string, ImageResource>& images() const;

	const std::unordered_map<std::string, MeshResource>& meshes() const;

	const std::unordered_map<std::string, MaterialResource>& materials() const;

	const std::unordered_map<std::string, VectorFieldResource>& vectorFields() const;

	// TODO

private:
	std::unordered_map<std::string, ImageResource> imageResources;
	std::unordered_map<std::string, MeshResource> meshResources;
	std::unordered_map<std::string, MaterialResource> materialResources;
	std::unordered_map<std::string, VectorFieldResource> vectorFieldResources;
};

void to_json(nlohmann::ordered_json& j, const ResourceCollection& resources);
void from_json(const nlohmann::ordered_json& j, ResourceCollection& resources);
}