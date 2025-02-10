#include "ComputeNode.h"

namespace pixelpart {
ComputeNode::InputException::InputException(const std::string& msg, uint32_t index) :
	std::runtime_error(msg), inputIndex(index) {

}

uint32_t ComputeNode::InputException::index() const {
	return inputIndex;
}

ComputeNode::Link::Link(id_t linkId, id_t linkNodeId, uint32_t linkSlot) :
	id(linkId), nodeId(linkNodeId), slot(linkSlot) {

}

ComputeNode::ComputeNode(
	const std::string& type,
	const std::string& category,
	const std::vector<std::string>& inputSlots,
	const std::vector<std::string>& outputSlots,
	const std::vector<Signature>& signatures,
	const std::vector<VariantValue>& defaultInputs,
	const std::vector<VariantParameter>& parameters) :
	nodeType(type), nodeCategory(category),
	nodeInputSlots(inputSlots), nodeOutputSlots(outputSlots),
	nodeSignatures(signatures), nodeDefaultInputs(defaultInputs),
	nodeParameters(parameters) {
	if(nodeDefaultInputs.size() != nodeInputSlots.size()) {
		throw InputException("Number of default input values does not match number of input slots");
	}

	for(const Signature& signature : nodeSignatures) {
		if(signature.inputTypes.size() != nodeInputSlots.size()) {
			throw InputException("Number of input types of a signature does not match number of input slots");
		}
		else if(signature.outputTypes.size() != nodeOutputSlots.size()) {
			throw InputException("Number of output types of a signature does not match number of output slots");
		}
	}

	nodeInputLinks.resize(nodeInputSlots.size());

	nodeParameterValues.resize(nodeParameters.size());
	for(std::size_t p = 0u; p < nodeParameters.size(); p++) {
		nodeParameterValues[p] = nodeParameters[p].def();
	}
}

std::unique_ptr<ComputeNode> ComputeNode::clone() const {
	std::unique_ptr<ComputeNode> clonedNode(cloneImpl());
	clonedNode->nodeInputLinks = nodeInputLinks;
	clonedNode->nodeParameterValues = nodeParameterValues;
	clonedNode->nodePosition = nodePosition;

	return clonedNode;
}

void ComputeNode::type(const std::string& type) {
	nodeType = type;
}
const std::string& ComputeNode::type() const {
	return nodeType;
}

void ComputeNode::category(const std::string& category) {
	nodeCategory = category;
}
const std::string& ComputeNode::category() const {
	return nodeCategory;
}

void ComputeNode::name(const std::string& name) {
	nodeName = name;
}
const std::string& ComputeNode::name() const {
	return nodeName;
}

std::vector<std::string>& ComputeNode::inputSlots() {
	return nodeInputSlots;
}
const std::vector<std::string>& ComputeNode::inputSlots() const {
	return nodeInputSlots;
}

std::vector<std::string>& ComputeNode::outputSlots() {
	return nodeOutputSlots;
}
const std::vector<std::string>& ComputeNode::outputSlots() const {
	return nodeOutputSlots;
}

std::vector<ComputeNode::Signature>& ComputeNode::signatures() {
	return nodeSignatures;
}
const std::vector<ComputeNode::Signature>& ComputeNode::signatures() const {
	return nodeSignatures;
}

std::vector<VariantValue>& ComputeNode::defaultInputs() {
	return nodeDefaultInputs;
}
const std::vector<VariantValue>& ComputeNode::defaultInputs() const {
	return nodeDefaultInputs;
}

std::vector<ComputeNode::Link>& ComputeNode::inputLinks() {
	return nodeInputLinks;
}
const std::vector<ComputeNode::Link>& ComputeNode::inputLinks() const {
	return nodeInputLinks;
}

std::vector<VariantParameter>& ComputeNode::parameters() {
	return nodeParameters;
}
const std::vector<VariantParameter>& ComputeNode::parameters() const {
	return nodeParameters;
}

std::vector<VariantParameter::Value>& ComputeNode::parameterValues() {
	return nodeParameterValues;
}
const std::vector<VariantParameter::Value>& ComputeNode::parameterValues() const {
	return nodeParameterValues;
}
const VariantParameter::Value& ComputeNode::parameterValue(std::size_t index) const {
	return nodeParameterValues.at(index);
}

uint32_t ComputeNode::findInputSlot(const std::string& slotName) const {
	uint32_t slot = id_t::nullValue;
	for(uint32_t o = 0u; o < nodeInputSlots.size(); o++) {
		if(nodeInputSlots[o] == slotName) {
			slot = o;
			break;
		}
	}

	return slot;
}
uint32_t ComputeNode::findOutputSlot(const std::string& slotName) const {
	uint32_t slot = id_t::nullValue;
	for(uint32_t o = 0u; o < nodeOutputSlots.size(); o++) {
		if(nodeOutputSlots[o] == slotName) {
			slot = o;
			break;
		}
	}

	return slot;
}
uint32_t ComputeNode::findParameter(const std::string& parameterName) const {
	uint32_t index = id_t::nullValue;
	for(uint32_t o = 0u; o < nodeParameters.size(); o++) {
		if(nodeParameters[o].name() == parameterName) {
			index = o;
			break;
		}
	}

	return index;
}

void ComputeNode::move(const float2_t& pos) {
	nodePosition = pos;
}
float2_t ComputeNode::position() const {
	return nodePosition;
}

void to_json(nlohmann::ordered_json& j, const ComputeNode::Signature& signature) {
	j = nlohmann::ordered_json{
		{ "input_types", signature.inputTypes },
		{ "output_types", signature.outputTypes }
	};
}
void to_json(nlohmann::ordered_json& j, const ComputeNode::Link& link) {
	j = nlohmann::ordered_json{
		{ "id", link.id },
		{ "node", link.nodeId },
		{ "slot", link.slot }
	};
}
void to_json(nlohmann::ordered_json& j, const ComputeNode& node) {
	j = nlohmann::ordered_json{
		{ "type", node.type() },
		{ "name", node.name() },
		{ "inputs", node.inputLinks() },
		{ "parameter_values", node.parameterValues() },
		{ "position", node.position() }
	};
}
void from_json(const nlohmann::ordered_json& j, ComputeNode::Signature& signature) {
	signature = ComputeNode::Signature();
	signature.inputTypes = j.value("input_types", std::vector<VariantValue::Type>());
	signature.outputTypes = j.value("output_types", std::vector<VariantValue::Type>());
}
void from_json(const nlohmann::ordered_json& j, ComputeNode::Link& link) {
	link = ComputeNode::Link(
		j.value("id", id_t()),
		j.value("node", id_t()),
		j.value("slot", id_t::nullValue));
}
void from_json(const nlohmann::ordered_json& j, ComputeNode& node) {
	node.name(j.value("name", ""));
	node.inputLinks() = j.value("inputs", std::vector<ComputeNode::Link>());
	node.parameterValues() = j.value("parameter_values", std::vector<VariantParameter::Value>());
	node.move(j.value("position", float2_t(0.0)));
}
}