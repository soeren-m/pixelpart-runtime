#pragma once

#include "ShaderNodeType.h"

namespace pixelpart {
struct ShaderGraphLanguage {
	std::vector<ShaderNodeType> nodes;

	std::vector<std::string> typeNames;
	std::vector<std::string> typeConstructors;
	std::vector<std::vector<std::string>> typeCasts;

	std::string parameterTemplate;
	std::vector<std::string> parameterTypeNames;
	std::vector<std::string> textureSamplers;

	std::string variablePrefix;
	std::string parameterPrefix;
	std::string parameterBlockName;
	std::string parameterBlockTemplate;

	std::string shaderTemplate;
};

void to_json(nlohmann::ordered_json& j, const ShaderGraphLanguage& graphLanguage);
void from_json(const nlohmann::ordered_json& j, ShaderGraphLanguage& graphLanguage);
}