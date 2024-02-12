#pragma once

#include "ShaderNodeType.h"

namespace pixelpart {
struct ShaderNode {
	struct Link {
		id_t id = nullId;
		id_t nodeId = nullId;
		uint32_t slot = 0u;

		Link();
		Link(id_t l, id_t n, uint32_t s);
	};

	ShaderNode();
	ShaderNode(const ShaderNodeType& nodeType);

	std::string type;
	std::string name;
	std::vector<Link> inputs;
	std::vector<VariantParameter::Value> parameters;
	bool isParameterNode = false;

	vec2_t position = vec2_t(0.0);
};

void to_json(nlohmann::ordered_json& j, const ShaderNode::Link& link);
void to_json(nlohmann::ordered_json& j, const ShaderNode& node);
void from_json(const nlohmann::ordered_json& j, ShaderNode::Link& link);
void from_json(const nlohmann::ordered_json& j, ShaderNode& node);
}