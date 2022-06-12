#pragma once

#include "ShaderNodeType.h"

namespace pixelpart {
struct ShaderGraphType {
	std::vector<ShaderNodeType> nodes;
	std::vector<std::string> typeNames;
	std::vector<std::string> typeConstructors;
	std::vector<std::string> typeCasts;
	std::vector<std::string> textureSamplers;
	std::string shaderTemplate;
};

void to_json(nlohmann::ordered_json& j, const ShaderGraphType& graphType);
void from_json(const nlohmann::ordered_json& j, ShaderGraphType& graphType);
}