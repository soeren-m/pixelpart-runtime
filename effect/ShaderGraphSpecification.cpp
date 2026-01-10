#include "ShaderGraphSpecification.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const ShaderGraphSpecification& specification) {
	j = nlohmann::ordered_json{
		{ "nodes", specification.nodes },
		{ "variable_prefix", specification.variablePrefix },
		{ "type_names", specification.typeNames },
		{ "type_constructors", specification.typeConstructors },
		{ "type_casts", specification.typeCasts },
		{ "parameter_prefix", specification.parameterPrefix },
		{ "parameter_definition", specification.parameterDefinition },
		{ "parameter_type_names", specification.parameterTypeNames },
		{ "texture_samplers", specification.textureSamplers }
	};
}
void from_json(const nlohmann::ordered_json& j, ShaderGraphSpecification& specification) {
	specification = ShaderGraphSpecification();
	specification.nodes = j.value("nodes", std::vector<ShaderNodeType>());
	specification.variablePrefix = j.value("variable_prefix", "");
	specification.typeNames = j.value("type_names", std::vector<std::string>());
	specification.typeConstructors = j.value("type_constructors", std::vector<std::string>());
	specification.typeCasts = j.value("type_casts", std::vector<std::vector<std::string>>());
	specification.parameterPrefix = j.value("parameter_prefix", "");
	specification.parameterDefinition = j.value("parameter_definition", "");
	specification.parameterTypeNames = j.value("parameter_type_names", std::vector<std::string>());
	specification.textureSamplers = j.value("texture_samplers", std::vector<std::string>());
}
}
