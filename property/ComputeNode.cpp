#include "ComputeNode.h"

namespace pixelpart {
ComputeNode::InputException::InputException(const std::string& msg, uint32_t index) :
	std::runtime_error(msg), inputIndex(index) {

}

uint32_t ComputeNode::InputException::getInputIndex() const {
	return inputIndex;
}

ComputeNode::Link::Link() {

}
ComputeNode::Link::Link(id_t l, id_t n, uint32_t s) : id(l), nodeId(n), slot(s) {

}

ComputeNode::ComputeNode(
	const std::string& nodeType,
	const std::string& nodeCategory,
	const std::vector<std::string>& nodeInputs,
	const std::vector<std::string>& nodeOutputs,
	const std::vector<Signature>& nodeSignatures,
	const std::vector<VariantValue>& nodeDefaultInputs,
	const std::vector<VariantParameter>& nodeParameters) :
	type(nodeType), category(nodeCategory),
	inputSlots(nodeInputs), outputSlots(nodeOutputs),
	signatures(nodeSignatures), defaultInputs(nodeDefaultInputs),
	parameters(nodeParameters) {
	if(defaultInputs.size() != inputSlots.size()) {
		throw InputException("Number of default input values does not match number of input slots");
	}

	for(const Signature& signature : signatures) {
		if(signature.inputTypes.size() != inputSlots.size()) {
			throw InputException("Number of input types of a signature does not match number of input slots");
		}
		else if(signature.outputTypes.size() != outputSlots.size()) {
			throw InputException("Number of output types of a signature does not match number of output slots");
		}
	}

	inputs.resize(inputSlots.size());

	parameterValues.resize(parameters.size());
	for(std::size_t p = 0u; p < parameters.size(); p++) {
		parameterValues[p] = parameters[p].defaultValue;
	}
}

std::unique_ptr<ComputeNode> ComputeNode::clone() {
	std::unique_ptr<ComputeNode> clonedNode(cloneImpl());
	clonedNode->inputs = inputs;
	clonedNode->parameterValues = parameterValues;
	clonedNode->position = position;

	return clonedNode;
}

void ComputeNode::setName(const std::string& nodeName) {
	name = nodeName;
}
void ComputeNode::setInputs(const std::vector<Link>& nodeInputs) {
	inputs = nodeInputs;
}
void ComputeNode::setParameterValues(const std::vector<VariantParameter::Value>& values) {
	parameterValues = values;
}

uint32_t ComputeNode::findInputSlot(const std::string& slotName) const {
	uint32_t slot = nullId;
	for(uint32_t o = 0u; o < inputSlots.size(); o++) {
		if(inputSlots[o] == slotName) {
			slot = o;
			break;
		}
	}

	return slot;
}
uint32_t ComputeNode::findOutputSlot(const std::string& slotName) const {
	uint32_t slot = nullId;
	for(uint32_t o = 0u; o < outputSlots.size(); o++) {
		if(outputSlots[o] == slotName) {
			slot = o;
			break;
		}
	}

	return slot;
}
uint32_t ComputeNode::findParameter(const std::string& parameterName) const {
	uint32_t index = nullId;
	for(uint32_t o = 0u; o < parameters.size(); o++) {
		if(parameters[o].name == parameterName) {
			index = o;
			break;
		}
	}

	return index;
}

const std::string& ComputeNode::getType() const {
	return type;
}
const std::string& ComputeNode::getName() const {
	return name;
}
const std::string& ComputeNode::getCategory() const {
	return category;
}

const std::vector<std::string>& ComputeNode::getInputSlots() const {
	return inputSlots;
}
const std::vector<std::string>& ComputeNode::getOutputSlots() const {
	return outputSlots;
}
const std::vector<ComputeNode::Signature>& ComputeNode::getSignatures() const {
	return signatures;
}
const std::vector<VariantValue>& ComputeNode::getDefaultInputs() const {
	return defaultInputs;
}
const std::vector<ComputeNode::Link>& ComputeNode::getInputs() const {
	return inputs;
}

const std::vector<VariantParameter>& ComputeNode::getParameters() const {
	return parameters;
}
const std::vector<VariantParameter::Value>& ComputeNode::getParameterValues() const {
	return parameterValues;
}

void ComputeNode::setPosition(const vec2_t& pos) {
	position = pos;
}
vec2_t ComputeNode::getPosition() const {
	return position;
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
		{ "type", node.getType() },
		{ "name", node.getName() },
		{ "inputs", node.getInputs() },
		{ "parameter_values", node.getParameterValues() },
		{ "position", node.getPosition() }
	};
}
void from_json(const nlohmann::ordered_json& j, ComputeNode::Signature& signature) {
	signature = ComputeNode::Signature();
	signature.inputTypes = j.at("input_types").get<std::vector<VariantValue::Type>>();
	signature.outputTypes = j.at("output_types").get<std::vector<VariantValue::Type>>();
}
void from_json(const nlohmann::ordered_json& j, ComputeNode::Link& link) {
	link = ComputeNode::Link();
	link.id = j.at("id");
	link.nodeId = j.at("node");
	link.slot = j.at("slot");
}
void from_json(const nlohmann::ordered_json& j, ComputeNode& node) {
	std::string name;
	std::vector<ComputeNode::Link> inputs;
	std::vector<VariantParameter::Value> parameterValues;
	vec2_t position = vec2_t(0.0);

	name = j.at("name");
	inputs = j.at("inputs");
	parameterValues = j.at("parameter_values");

	if(j.contains("position")) {
		position = j.at("position");
	}

	node.setName(name);
	node.setInputs(inputs);
	node.setParameterValues(parameterValues);
	node.setPosition(position);
}
}