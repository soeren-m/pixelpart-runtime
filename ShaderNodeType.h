#pragma once

#include "ShaderValueType.h"
#include "ShaderParameter.h"

namespace pixelpart {
struct ShaderNodeType {
	struct Signature {
		std::vector<ShaderValueType> inputTypes;
		std::vector<ShaderValueType> outputTypes;
	};

	std::string name;
	std::string category;
	std::string code;
	std::vector<std::string> inputs;
	std::vector<std::string> outputs;
	std::vector<Signature> signatures;
	std::vector<ShaderParameter> parameters;
};

void to_json(nlohmann::ordered_json& j, const ShaderNodeType::Signature& signature);
void to_json(nlohmann::ordered_json& j, const ShaderNodeType& nodeType);
void from_json(const nlohmann::ordered_json& j, ShaderNodeType::Signature& signature);
void from_json(const nlohmann::ordered_json& j, ShaderNodeType& nodeType);
}