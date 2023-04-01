#include "ShaderNode.h"
#include "JSONUtil.h"

namespace pixelpart {
ShaderNode::Link::Link() {

}
ShaderNode::Link::Link(uint32_t l, uint32_t n, uint32_t s) : id(l), nodeId(n), slot(s) {

}

ShaderNode::ShaderNode() {

}
ShaderNode::ShaderNode(const ShaderNodeType& nodeType) : type(nodeType.name), inputs(nodeType.inputs.size()), parameters(nodeType.parameters.size()) {
	for(std::size_t p = 0; p < parameters.size(); p++) {
		parameters[p] = nodeType.parameters[p].defaultValue;
	}
}

void to_json(nlohmann::ordered_json& j, const ShaderNode::Link& link) {
	j = nlohmann::ordered_json{
		{ "id", link.id },
		{ "node", link.nodeId },
		{ "slot", link.slot },
	};
}
void to_json(nlohmann::ordered_json& j, const ShaderNode& node) {
	j = nlohmann::ordered_json{
		{ "type", node.type },
		{ "name", node.name },
		{ "inputs", node.inputs },
		{ "parameters", node.parameters },
		{ "position", node.position }
	};
}
void from_json(const nlohmann::ordered_json& j, ShaderNode::Link& link) {
	link = ShaderNode::Link();

	fromJson(link.id, j, "id");
	fromJson(link.nodeId, j, "node");
	fromJson(link.slot, j, "slot");
}
void from_json(const nlohmann::ordered_json& j, ShaderNode& node) {
	node = ShaderNode();

	fromJson(node.type, j, "type");
	fromJson(node.name, j, "name");
	fromJson(node.inputs , j, "inputs");
	fromJson(node.parameters, j, "parameters");
	fromJson(node.position, j, "position");
}
}