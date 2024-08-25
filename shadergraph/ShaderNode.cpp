#include "ShaderNode.h"

namespace pixelpart {
ShaderNode::ShaderNode(const ShaderNodeType& nodeType) :
	nodeTypeId(nodeType.name), nodeInputs(nodeType.inputs.size()), nodeParameters(nodeType.parameters.size()) {
	for(std::size_t p = 0u; p < nodeParameters.size(); p++) {
		nodeParameters[p] = nodeType.parameters[p].def();
	}
}
ShaderNode::ShaderNode(const std::string& typeId, const std::string& name,
	const std::vector<Link>& inputs,
	const std::vector<VariantParameter::Value>& parameters,
	bool parameterNode, const vec2_t& position) :
	nodeTypeId(typeId), nodeName(name), nodeInputs(inputs), nodeParameters(parameters),
	nodeIsParameterNode(parameterNode), nodePosition(position) {

}

const std::string& ShaderNode::type() const {
	return nodeTypeId;
}
const std::string& ShaderNode::name() const {
	return nodeName;
}
const std::vector<ShaderNode::Link>& ShaderNode::inputs() const {
	return nodeInputs;
}
const std::vector<VariantParameter::Value>& ShaderNode::parameters() const {
	return nodeParameters;
}
bool ShaderNode::parameterNode() const {
	return nodeIsParameterNode;
}

const vec2_t& ShaderNode::position() const {
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
	link = ShaderNode::Link();
	link.id = j.at("id");
	link.nodeId = j.at("node");
	link.slot = j.value("slot", 0u);
}
void from_json(const nlohmann::ordered_json& j, ShaderNode& node) {
	node = ShaderNode(
		j.at("type"),
		j.value("name", ""),
		j.value("inputs", std::vector<ShaderNode::Link>()),
		j.value("parameters", std::vector<VariantParameter::Value>()),
		j.value("parameter_node", false),
		j.value("position", vec2_t(0.0)));
}
}