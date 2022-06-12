#pragma once

#include "ShaderNodeType.h"

namespace pixelpart {
struct ShaderNode {
	struct Link {
		uint32_t id = NullId;
		uint32_t nodeId = NullId;
		uint32_t slot = 0;

		Link();
		Link(uint32_t l, uint32_t n, uint32_t s);
	};

	ShaderNode();
	ShaderNode(const ShaderNodeType& nodeType);

	std::string type;
	std::vector<Link> inputs;
	std::vector<ShaderParameter::Value> parameters;

	vec2d position = vec2d(0.0);
};

void to_json(nlohmann::ordered_json& j, const ShaderNode::Link& link);
void to_json(nlohmann::ordered_json& j, const ShaderNode& node);
void from_json(const nlohmann::ordered_json& j, ShaderNode::Link& link);
void from_json(const nlohmann::ordered_json& j, ShaderNode& node);
}