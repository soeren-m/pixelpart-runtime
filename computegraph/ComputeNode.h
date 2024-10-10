#pragma once

#include "../common/Types.h"
#include "../common/Math.h"
#include "../common/Id.h"
#include "../common/VariantValue.h"
#include "../common/VariantParameter.h"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

namespace pixelpart {
class ComputeNode {
	friend class ComputeGraph;

public:
	class InputException : public std::runtime_error {
	public:
		InputException(const std::string& msg, uint32_t index = id_t::nullValue);

		uint32_t index() const;

	private:
		uint32_t inputIndex = id_t::nullValue;
	};

	struct Signature {
		std::vector<VariantValue::Type> inputTypes;
		std::vector<VariantValue::Type> outputTypes;
	};
	struct Link {
		Link() = default;
		Link(id_t linkId, id_t linkNodeId, uint32_t linkSlot);

		id_t id;
		id_t nodeId;
		uint32_t slot = id_t::nullValue;
	};

	ComputeNode(
		const std::string& type,
		const std::string& category,
		const std::vector<std::string>& inputSlots,
		const std::vector<std::string>& ouputSlots,
		const std::vector<Signature>& signatures,
		const std::vector<VariantValue>& defaultInputs,
		const std::vector<VariantParameter>& parameters);
	virtual ~ComputeNode() = default;

	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const = 0;

	std::unique_ptr<ComputeNode> clone();

	void type(const std::string& type);
	const std::string& type() const;

	void category(const std::string& category);
	const std::string& category() const;

	void name(const std::string& name);
	const std::string& name() const;

	std::vector<std::string>& inputSlots();
	const std::vector<std::string>& inputSlots() const;

	std::vector<std::string>& outputSlots();
	const std::vector<std::string>& outputSlots() const;

	std::vector<Signature>& signatures();
	const std::vector<Signature>& signatures() const;

	std::vector<VariantValue>& defaultInputs();
	const std::vector<VariantValue>& defaultInputs() const;

	std::vector<Link>& inputLinks();
	const std::vector<Link>& inputLinks() const;

	std::vector<VariantParameter>& parameters();
	const std::vector<VariantParameter>& parameters() const;

	std::vector<VariantParameter::Value>& parameterValues();
	const std::vector<VariantParameter::Value>& parameterValues() const;
	const VariantParameter::Value& parameterValue(std::size_t index) const;

	uint32_t findInputSlot(const std::string& slotName) const;
	uint32_t findOutputSlot(const std::string& slotName) const;
	uint32_t findParameter(const std::string& parameterName) const;

	void move(const float2_t& pos);
	float2_t position() const;

protected:
	virtual ComputeNode* cloneImpl() const = 0;

	std::string nodeType;
	std::string nodeCategory;
	std::string nodeName;

	std::vector<std::string> nodeInputSlots;
	std::vector<std::string> nodeOutputSlots;
	std::vector<Signature> nodeSignatures;
	std::vector<VariantValue> nodeDefaultInputs;
	std::vector<Link> nodeInputLinks;

	std::vector<VariantParameter> nodeParameters;
	std::vector<VariantParameter::Value> nodeParameterValues;

	float2_t nodePosition = float2_t(0.0);
};

void to_json(nlohmann::ordered_json& j, const ComputeNode::Signature& signature);
void to_json(nlohmann::ordered_json& j, const ComputeNode::Link& link);
void to_json(nlohmann::ordered_json& j, const ComputeNode& node);
void from_json(const nlohmann::ordered_json& j, ComputeNode::Signature& signature);
void from_json(const nlohmann::ordered_json& j, ComputeNode::Link& link);
void from_json(const nlohmann::ordered_json& j, ComputeNode& node);
}