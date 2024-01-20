#include "MaterialResource.h"
#include "../common/JsonUtil.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const MaterialResource& resource) {
	j = nlohmann::ordered_json{
		{ "name", resource.name },
		{ "shader_graph", resource.shaderGraph }
	};
}
void from_json(const nlohmann::ordered_json& j, MaterialResource& resource) {
	resource = MaterialResource();

	fromJson(resource.name, j, "name");
	fromJson(resource.shaderGraph, j, "shader_graph");
}
}