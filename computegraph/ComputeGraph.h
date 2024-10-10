#pragma once

#include "../common/Types.h"
#include "../common/VariantValue.h"
#include "../common/Id.h"
#include "ComputeNode.h"
#include "ComputeNodeFactory.h"
#include "../json/json.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

namespace pixelpart {
class ComputeGraph {
public:
	using ComputeNodeCollection = std::unordered_map<id_t, std::unique_ptr<ComputeNode>>;
	using InputSet = std::unordered_map<id_t, VariantValue>;

	class EvaluationException : public std::runtime_error {
	public:
		EvaluationException(const std::string& msg, id_t node = id_t(), uint32_t slot = id_t::nullValue);

		id_t nodeId() const;
		uint32_t slotIndex() const;

	private:
		id_t computeNodeId;
		uint32_t computeSlotIndex;
	};

	enum TypeMatch : uint32_t {
		typematch_exact,
		typematch_upcast,
		typematch_downcast,
		typematch_none
	};

	struct BuildResult {
		std::unordered_map<id_t, uint32_t> nodeSignatures;
		std::unordered_map<id_t, std::vector<VariantValue>> nodeOutputs;
	};

	static ComputeNodeFactory nodeFactory;

	ComputeGraph() = default;
	ComputeGraph(const ComputeGraph& other);
	ComputeGraph(const ComputeNodeCollection& initialNodes);

	ComputeGraph& operator=(const ComputeGraph& other);

	std::vector<VariantValue> evaluate(const InputSet& graphInputs) const;
	std::vector<VariantValue> evaluate(const InputSet& graphInputs, BuildResult& result, id_t nodeId = id_t(0u)) const;

	template <typename TNode>
	id_t addNode() {
		id_t nodeId = nextNodeId;
		computeNodes[nodeId] = std::unique_ptr<TNode>(new TNode());
		nextNodeId++;

		return nodeId;
	}

	id_t addNode(const std::string& typeName);
	void removeNode(id_t nodeId);
	bool containsNode(id_t nodeId) const;

	void linkNodes(id_t sourceNodeId, id_t targetNodeId, uint32_t sourceSlot, uint32_t targetSlot);
	void linkNodes(id_t sourceNodeId, id_t targetNodeId, const std::string& sourceSlotName, const std::string& targetSlotName);
	void linkNodeToInput(id_t targetNodeId, uint32_t targetSlot, id_t inputId);
	void unlinkNodes(id_t sourceNodeId, id_t targetNodeId, uint32_t targetSlot);
	void unlinkNodes(id_t linkId);
	void unlinkRemovedInputs(const InputSet& graphInputs);

	void nodeName(id_t nodeId, const std::string& name);
	void nodeParameter(id_t nodeId, uint32_t parameterIndex, VariantParameter::Value value);
	void nodeParameter(id_t nodeId, const std::string& parameterName, VariantParameter::Value value);
	void nodePosition(id_t nodeId, const float2_t& position);

	const ComputeNode& node(id_t nodeId) const;
	const ComputeNodeCollection& nodes() const;

	bool empty() const;

private:
	uint32_t findNodeType(const std::string& typeName) const;
	uint32_t findNodeSignature(const InputSet& graphInputs, const BuildResult& result, const ComputeNode& activeNode, std::vector<TypeMatch>& typeMatch) const;

	ComputeNodeCollection computeNodes;
	id_t nextNodeId = id_t(0u);
	id_t nextLinkId = id_t(0u);
};

void to_json(nlohmann::ordered_json& j, const ComputeGraph& computeGraph);
void from_json(const nlohmann::ordered_json& j, ComputeGraph& computeGraph);
}