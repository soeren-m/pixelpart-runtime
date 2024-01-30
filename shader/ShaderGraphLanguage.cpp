#include "ShaderGraphLanguage.h"
#include "../common/JsonUtil.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const ShaderGraphLanguage& graphLanguage) {
	j = nlohmann::ordered_json{
		{ "nodes", graphLanguage.nodes },

		{ "type_names", graphLanguage.typeNames },
		{ "type_constructors", graphLanguage.typeConstructors },
		{ "type_casts", graphLanguage.typeCasts },

		{ "parameter_template", graphLanguage.parameterTemplate },
		{ "parameter_type_names", graphLanguage.parameterTypeNames },
		{ "texture_samplers", graphLanguage.textureSamplers },

		{ "variable_prefix", graphLanguage.variablePrefix },
		{ "parameter_prefix", graphLanguage.parameterPrefix },
		{ "parameter_block_name", graphLanguage.parameterBlockName },
		{ "parameter_block_template", graphLanguage.parameterBlockTemplate },

		{ "shader_template", graphLanguage.shaderTemplate }
	};
}
void from_json(const nlohmann::ordered_json& j, ShaderGraphLanguage& graphLanguage) {
	graphLanguage = ShaderGraphLanguage();

	fromJson(graphLanguage.nodes, j, "nodes");

	fromJson(graphLanguage.typeNames, j, "type_names");
	fromJson(graphLanguage.typeConstructors, j, "type_constructors");
	fromJson(graphLanguage.typeCasts, j, "type_casts");

	fromJson(graphLanguage.parameterTemplate, j, "parameter_template");
	fromJson(graphLanguage.parameterTypeNames, j, "parameter_type_names");
	fromJson(graphLanguage.textureSamplers, j, "texture_samplers");

	fromJson(graphLanguage.variablePrefix, j, "variable_prefix");
	fromJson(graphLanguage.parameterPrefix, j, "parameter_prefix");
	fromJson(graphLanguage.parameterBlockName, j, "parameter_block_name");
	fromJson(graphLanguage.parameterBlockTemplate, j, "parameter_block_template");

	fromJson(graphLanguage.shaderTemplate, j, "shader_template");
}
}