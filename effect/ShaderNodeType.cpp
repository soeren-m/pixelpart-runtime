#include "ShaderNodeType.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const ShaderNodeType::Signature& signature) {
	j = nlohmann::ordered_json{
		{ "input_types", signature.inputTypes },
		{ "output_types", signature.outputTypes }
	};
}
void to_json(nlohmann::ordered_json& j, const ShaderNodeType& nodeType) {
	j = nlohmann::ordered_json{
		{ "name", nodeType.name },
		{ "category", nodeType.category },
		{ "code", nodeType.code },
		{ "inputs", nodeType.inputs },
		{ "outputs", nodeType.outputs },
		{ "signatures", nodeType.signatures },
		{ "default_inputs", nodeType.defaultInputs },
		{ "parameters", nodeType.parameters }
	};
}
void from_json(const nlohmann::ordered_json& j, ShaderNodeType::Signature& signature) {
	signature = ShaderNodeType::Signature();
	signature.inputTypes = j.value("input_types", std::vector<VariantValue::Type>());
	signature.outputTypes = j.value("output_types", std::vector<VariantValue::Type>());
}
void from_json(const nlohmann::ordered_json& j, ShaderNodeType& nodeType) {
	nodeType = ShaderNodeType();
	nodeType.name = j.at("name");
	nodeType.category = j.value("category", "");
	nodeType.code = j.value("code", "");
	nodeType.inputs = j.value("inputs", std::vector<std::string>());
	nodeType.outputs = j.value("outputs", std::vector<std::string>());
	nodeType.signatures = j.value("signatures", std::vector<ShaderNodeType::Signature>());
	nodeType.defaultInputs = j.value("default_inputs", std::vector<VariantValue>());
	nodeType.defaultInputs.resize(nodeType.inputs.size());
	nodeType.parameters = j.value("parameters", std::vector<VariantParameter>());
}
}
