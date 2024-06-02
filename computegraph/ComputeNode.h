#pragma once

#include "../common/VariantValue.h"
#include "../common/VariantParameter.h"
#include "../common/Constants.h"
#include <memory>
#include <unordered_map>
#include <stdexcept>

namespace pixelpart {
class ComputeNode {
	friend class ComputeGraph;

public:
	class InputException : public std::runtime_error {
	public:
		InputException(const std::string& msg, uint32_t index = nullId);

		uint32_t getInputIndex() const;

	private:
		uint32_t inputIndex = nullId;
	};

	struct Signature {
		std::vector<VariantValue::Type> inputTypes;
		std::vector<VariantValue::Type> outputTypes;
	};
	struct Link {
		id_t id = nullId;
		id_t nodeId = nullId;
		uint32_t slot = nullId;

		Link();
		Link(id_t l, id_t n, uint32_t s);
	};

	ComputeNode(
		const std::string& nodeType,
		const std::string& nodeCategory,
		const std::vector<std::string>& nodeInputs,
		const std::vector<std::string>& nodeOutputs,
		const std::vector<Signature>& nodeSignatures,
		const std::vector<VariantValue>& nodeDefaultInputs,
		const std::vector<VariantParameter>& nodeParameters);
	virtual ~ComputeNode() = default;

	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const = 0;

	std::unique_ptr<ComputeNode> clone();

	void setName(const std::string& nodeName);
	void setInputs(const std::vector<Link>& nodeInputs);
	void setParameterValues(const std::vector<VariantParameter::Value>& values);

	uint32_t findInputSlot(const std::string& slotName) const;
	uint32_t findOutputSlot(const std::string& slotName) const;
	uint32_t findParameter(const std::string& parameterName) const;

	const std::string& getType() const;
	const std::string& getName() const;
	const std::string& getCategory() const;

	const std::vector<std::string>& getInputSlots() const;
	const std::vector<std::string>& getOutputSlots() const;
	const std::vector<Signature>& getSignatures() const;
	const std::vector<VariantValue>& getDefaultInputs() const;
	const std::vector<Link>& getInputs() const;

	const std::vector<VariantParameter>& getParameters() const;
	const std::vector<VariantParameter::Value>& getParameterValues() const;

	void setPosition(const vec2_t& pos);
	vec2_t getPosition() const;

protected:
	virtual ComputeNode* cloneImpl() const = 0;

	std::string type;
	std::string name;
	std::string category;

	std::vector<std::string> inputSlots;
	std::vector<std::string> outputSlots;
	std::vector<Signature> signatures;
	std::vector<VariantValue> defaultInputs;
	std::vector<Link> inputs;

	std::vector<VariantParameter> parameters;
	std::vector<VariantParameter::Value> parameterValues;

	vec2_t position = vec2_t(0.0);
};

void to_json(nlohmann::ordered_json& j, const ComputeNode::Signature& signature);
void to_json(nlohmann::ordered_json& j, const ComputeNode::Link& link);
void to_json(nlohmann::ordered_json& j, const ComputeNode& node);
void from_json(const nlohmann::ordered_json& j, ComputeNode::Signature& signature);
void from_json(const nlohmann::ordered_json& j, ComputeNode::Link& link);
void from_json(const nlohmann::ordered_json& j, ComputeNode& node);
}