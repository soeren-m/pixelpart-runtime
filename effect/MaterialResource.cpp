#include "MaterialResource.h"
#include "../common/Json.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const MaterialResource& resource) {
	j = nlohmann::ordered_json{
		{ "name", resource.name },
		{ "blend_mode", resource.blendMode },
		{ "lighting_mode", resource.lightingMode },
		{ "shader_graph", resource.shaderGraph }
	};
}
void from_json(const nlohmann::ordered_json& j, MaterialResource& resource) {
	resource = MaterialResource();

	fromJson(resource.name, j, "name");
	fromJson(resource.blendMode, j, "blend_mode");
	fromJson(resource.lightingMode, j, "lighting_mode");
	fromJson(resource.shaderGraph, j, "shader_graph");
}
}