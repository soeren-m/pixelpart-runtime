#pragma once

#include "../common/Types.h"
#include "../common/Math.h"
#include "../common/Id.h"
#include "../common/VariantParameter.h"
#include "ShaderNodeType.h"
#include "../json/json.hpp"
#include <string>
#include <vector>

namespace pixelpart {
class ShaderNode {
public:
	struct Link {
		Link() = default;
		Link(id_t linkId, id_t linkNodeId, std::uint32_t slotIndex);

		id_t id;
		id_t nodeId;
		std::uint32_t slot = 0;
	};

	ShaderNode() = default;
	ShaderNode(const ShaderNodeType& nodeType);
	ShaderNode(const std::string& typeId, const std::string& name,
		const std::vector<Link>& inputs,
		const std::vector<VariantParameter::Value>& parameters,
		bool parameterNode, const float2_t& position);

	const std::string& type() const;

	void name(const std::string& name);
	const std::string& name() const;

	std::vector<Link>& inputs();
	const std::vector<Link>& inputs() const;

	std::vector<VariantParameter::Value>& parameters();
	const std::vector<VariantParameter::Value>& parameters() const;

	void parameterNode(bool parameterNode);
	bool parameterNode() const;

	void position(const float2_t& position);
	const float2_t& position() const;

private:
	std::string nodeTypeId;
	std::string nodeName;
	std::vector<Link> nodeInputs;
	std::vector<VariantParameter::Value> nodeParameters;

	bool nodeIsParameterNode = false;

	float2_t nodePosition = float2_t(0.0);
};

void to_json(nlohmann::ordered_json& j, const ShaderNode::Link& link);
void to_json(nlohmann::ordered_json& j, const ShaderNode& node);
void from_json(const nlohmann::ordered_json& j, ShaderNode::Link& link);
void from_json(const nlohmann::ordered_json& j, ShaderNode& node);
}
