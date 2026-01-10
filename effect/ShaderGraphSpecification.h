#pragma once

#include "ShaderNodeType.h"
#include "../json/json.hpp"
#include <string>
#include <vector>

namespace pixelpart {
struct ShaderGraphSpecification {
	std::vector<ShaderNodeType> nodes;
	std::string variablePrefix;
	std::vector<std::string> typeNames;
	std::vector<std::string> typeConstructors;
	std::vector<std::vector<std::string>> typeCasts;
	std::string parameterPrefix;
	std::string parameterDefinition;
	std::vector<std::string> parameterTypeNames;
	std::vector<std::string> textureSamplers;
};

void to_json(nlohmann::ordered_json& j, const ShaderGraphSpecification& specification);
void from_json(const nlohmann::ordered_json& j, ShaderGraphSpecification& specification);
}
