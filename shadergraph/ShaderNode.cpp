#include "ShaderNode.h"

namespace pixelpart {
ShaderNode::Link::Link(id_t linkId, id_t linkNodeId, std::uint32_t slotIndex) :
	id(linkId), nodeId(linkNodeId), slot(slotIndex) {

}

ShaderNode::ShaderNode(const ShaderNodeType& nodeType) :
	nodeTypeId(nodeType.name), nodeInputs(nodeType.inputs.size()), nodeParameters(nodeType.parameters.size()) {
	for(std::size_t parameterIndex = 0; parameterIndex < nodeParameters.size(); parameterIndex++) {
		nodeParameters[parameterIndex] = nodeType.parameters[parameterIndex].def();
	}
}
ShaderNode::ShaderNode(const std::string& typeId, const std::string& name,
	const std::vector<Link>& inputs,
	const std::vector<VariantParameter::Value>& parameters,
	bool parameterNode, const float2_t& position) :
	nodeTypeId(typeId), nodeName(name), nodeInputs(inputs), nodeParameters(parameters),
	nodeIsParameterNode(parameterNode), nodePosition(position) {

}

const std::string& ShaderNode::type() const {
	return nodeTypeId;
}

void ShaderNode::name(const std::string& name) {
	nodeName = name;
}
const std::string& ShaderNode::name() const {
	return nodeName;
}

std::vector<ShaderNode::Link>& ShaderNode::inputs() {
	return nodeInputs;
}
const std::vector<ShaderNode::Link>& ShaderNode::inputs() const {
	return nodeInputs;
}

std::vector<VariantParameter::Value>& ShaderNode::parameters() {
	return nodeParameters;
}
const std::vector<VariantParameter::Value>& ShaderNode::parameters() const {
	return nodeParameters;
}

void ShaderNode::parameterNode(bool parameterNode) {
	nodeIsParameterNode = parameterNode;
}
bool ShaderNode::parameterNode() const {
	return nodeIsParameterNode;
}

void ShaderNode::position(const float2_t& position) {
	nodePosition = position;
}
const float2_t& ShaderNode::position() const {
	return nodePosition;
}

void to_json(nlohmann::ordered_json& j, const ShaderNode::Link& link) {
	j = nlohmann::ordered_json{
		{ "id", link.id },
		{ "node", link.nodeId },
		{ "slot", link.slot }
	};
}
void to_json(nlohmann::ordered_json& j, const ShaderNode& node) {
	j = nlohmann::ordered_json{
		{ "type", node.type() },
		{ "name", node.name() },
		{ "inputs", node.inputs() },
		{ "parameters", node.parameters() },
		{ "parameter_node", node.parameterNode() },
		{ "position", node.position() }
	};
}
void from_json(const nlohmann::ordered_json& j, ShaderNode::Link& link) {
	link = ShaderNode::Link(
		j.at("id"),
		j.at("node"),
		j.value("slot", 0u));
}
void from_json(const nlohmann::ordered_json& j, ShaderNode& node) {
	node = ShaderNode(
		j.at("type"),
		j.value("name", ""),
		j.value("inputs", std::vector<ShaderNode::Link>()),
		j.value("parameters", std::vector<VariantParameter::Value>()),
		j.value("parameter_node", false),
		j.value("position", float2_t(0.0)));
}
}