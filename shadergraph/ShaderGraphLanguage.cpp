#include "ShaderGraphLanguage.h"

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
	graphLanguage.nodes = j.value("nodes", std::vector<ShaderNodeType>());
	graphLanguage.variablePrefix = j.value("variable_prefix", "");
	graphLanguage.typeNames = j.value("type_names", std::vector<std::string>());
	graphLanguage.typeConstructors = j.value("type_constructors", std::vector<std::string>());
	graphLanguage.typeCasts = j.value("type_casts", std::vector<std::vector<std::string>>());
	graphLanguage.parameterPrefix = j.value("parameter_prefix", "");
	graphLanguage.parameterDefinition = j.value("parameter_definition", "");
	graphLanguage.parameterTypeNames = j.value("parameter_type_names", std::vector<std::string>());
	graphLanguage.textureSamplers = j.value("texture_samplers", std::vector<std::string>());
}
}