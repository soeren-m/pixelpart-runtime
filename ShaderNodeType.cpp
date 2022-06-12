#include "ShaderNodeType.h"
#include "JSONUtil.h"

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
		{ "parameters", nodeType.parameters }
	};
}
void from_json(const nlohmann::ordered_json& j, ShaderNodeType::Signature& signature) {
	signature = ShaderNodeType::Signature();
	fromJson(signature.inputTypes, j, "input_types", "");
	fromJson(signature.outputTypes, j, "output_types", "");
}
void from_json(const nlohmann::ordered_json& j, ShaderNodeType& nodeType) {
	nodeType = ShaderNodeType();
	fromJson(nodeType.name, j, "name", "");
	fromJson(nodeType.category, j, "category", "");
	fromJson(nodeType.code, j, "code", "");
	fromJson(nodeType.inputs, j, "inputs", "");
	fromJson(nodeType.outputs,  j, "outputs", "");
	fromJson(nodeType.signatures, j, "signatures", "");
	fromJson(nodeType.parameters, j, "parameters", "");
}
}