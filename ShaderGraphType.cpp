#include "ShaderGraphType.h"
#include "JSONUtil.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const ShaderGraphType& graphType) {
	j = nlohmann::ordered_json{
		{ "nodes", graphType.nodes },
		{ "type_names", graphType.typeNames },
		{ "type_constructors", graphType.typeConstructors },
		{ "type_casts", graphType.typeCasts },
		{ "texture_samplers", graphType.textureSamplers },
		{ "shader_template", graphType.shaderTemplate }
	};
}
void from_json(const nlohmann::ordered_json& j, ShaderGraphType& graphType) {
	graphType = ShaderGraphType();

	fromJson(graphType.nodes, j, "nodes");
	fromJson(graphType.typeNames, j, "type_names");
	fromJson(graphType.typeConstructors, j, "type_constructors");
	fromJson(graphType.typeCasts, j, "type_casts");
	fromJson(graphType.textureSamplers, j, "texture_samplers");
	fromJson(graphType.shaderTemplate, j, "shader_template");
}
}