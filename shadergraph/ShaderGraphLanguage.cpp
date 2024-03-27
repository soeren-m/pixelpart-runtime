#include "ShaderGraphLanguage.h"
#include "../common/Json.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const ShaderGraphLanguage& graphLanguage) {
	j = nlohmann::ordered_json{
		{ "nodes", graphLanguage.nodes },

		{ "variable_prefix", graphLanguage.variablePrefix },

		{ "type_names", graphLanguage.typeNames },
		{ "type_constructors", graphLanguage.typeConstructors },
		{ "type_casts", graphLanguage.typeCasts },

		{ "parameter_prefix", graphLanguage.parameterPrefix },
		{ "parameter_definition", graphLanguage.parameterDefinition },
		{ "parameter_type_names", graphLanguage.parameterTypeNames },

		{ "texture_samplers", graphLanguage.textureSamplers }
	};
}
void from_json(const nlohmann::ordered_json& j, ShaderGraphLanguage& graphLanguage) {
	graphLanguage = ShaderGraphLanguage();

	fromJson(graphLanguage.nodes, j, "nodes");

	fromJson(graphLanguage.variablePrefix, j, "variable_prefix");

	fromJson(graphLanguage.typeNames, j, "type_names");
	fromJson(graphLanguage.typeConstructors, j, "type_constructors");
	fromJson(graphLanguage.typeCasts, j, "type_casts");

	fromJson(graphLanguage.parameterPrefix, j, "parameter_prefix");
	fromJson(graphLanguage.parameterDefinition, j, "parameter_definition");
	fromJson(graphLanguage.parameterTypeNames, j, "parameter_type_names");

	fromJson(graphLanguage.textureSamplers, j, "texture_samplers");
}
}