#pragma once

#include "ComputeNodeFactory.h"
#include <memory>

namespace pixelpart {
class ComputeGraph {
public:
	using InputSet = std::unordered_map<uint32_t, VariantValue>;

	class EvaluationException : public std::runtime_error {
	public:
		EvaluationException(const std::string& msg, uint32_t node = nullId, uint32_t slot = nullId);

		uint32_t getNodeId() const;
		uint32_t getSlotIndex() const;

	private:
		uint32_t nodeId = nullId;
		uint32_t slotIndex = nullId;
	};

	enum TypeMatch : uint32_t {
		typematch_exact = 0,
		typematch_upcast,
		typematch_downcast,
		typematch_none
	};

	struct BuildResult {
		std::unordered_map<uint32_t, uint32_t> nodeSignatures;
		std::unordered_map<uint32_t, std::vector<VariantValue>> nodeOutputs;
	};

	static ComputeNodeFactory nodeFactory;

	ComputeGraph();
	ComputeGraph(const ComputeGraph& other);
	ComputeGraph(const std::unordered_map<uint32_t, std::unique_ptr<ComputeNode>>& initialNodes);

	ComputeGraph& operator=(const ComputeGraph& other);

	std::vector<VariantValue> evaluate(const InputSet& graphInputs) const;
	std::vector<VariantValue> evaluate(const InputSet& graphInputs, BuildResult& result, uint32_t nodeId = 0u) const;

	template <typename TNode>
	uint32_t addNode() {
		uint32_t nodeId = nextNodeId;
		nodes[nodeId] = std::unique_ptr<TNode>(new TNode());
		nextNodeId++;

		return nodeId;
	}

	uint32_t addNode(const std::string& typeName);
	void removeNode(uint32_t nodeId);

	void linkNodes(uint32_t sourceNodeId, uint32_t targetNodeId, uint32_t sourceSlot, uint32_t targetSlot);
	void linkNodes(uint32_t sourceNodeId, uint32_t targetNodeId, const std::string& sourceSlotName, const std::string& targetSlotName);
	void linkNodeToInput(uint32_t targetNodeId, uint32_t targetSlot, uint32_t inputId);
	void unlinkNodes(uint32_t sourceNodeId, uint32_t targetNodeId, uint32_t targetSlot);
	void unlinkNodes(uint32_t linkId);
	void unlinkRemovedInputs(const InputSet& graphInputs);

	void setNodeName(uint32_t nodeId, const std::string& name);
	void setNodeParameter(uint32_t nodeId, uint32_t parameterIndex, ComputeNodeParameter::Value value);
	void setNodeParameter(uint32_t nodeId, const std::string& parameterName, ComputeNodeParameter::Value value);
	void setNodePosition(uint32_t nodeId, const vec2d& position);

	bool hasNode(uint32_t nodeId) const;
	const ComputeNode& getNode(uint32_t nodeId) const;
	const std::unordered_map<uint32_t, std::unique_ptr<ComputeNode>>& getNodes() const;

	uint32_t getNextNodeId() const;
	uint32_t getNextLinkId() const;

	bool isEmpty() const;

	const ComputeNodeFactory& getNodeFactory() const;

private:
	uint32_t findNodeType(const std::string& typeName) const;
	uint32_t findNodeSignature(const InputSet& graphInputs, const BuildResult& result, const ComputeNode& node, std::vector<TypeMatch>& typeMatch) const;

	std::unordered_map<uint32_t, std::unique_ptr<ComputeNode>> nodes;
	uint32_t nextNodeId = 0u;
	uint32_t nextLinkId = 0u;
};

void to_json(nlohmann::ordered_json& j, const ComputeGraph& computeGraph);
void from_json(const nlohmann::ordered_json& j, ComputeGraph& computeGraph);
}