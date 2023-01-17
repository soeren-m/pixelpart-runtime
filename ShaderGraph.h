#pragma once

#include "ShaderNode.h"
#include "ShaderGraphType.h"
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>

namespace pixelpart {
class ShaderGraph {
public:
	class BuildException : public std::runtime_error {
	public:
		BuildException(const std::string& msg, uint32_t node = nullId, uint32_t slot = nullId);

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
		std::string code;
		std::unordered_map<uint32_t, std::vector<std::string>> nodeOutputVariables;
		std::unordered_map<uint32_t, uint32_t> nodeSignatures;
		std::unordered_set<uint32_t> resolvedNodes;
		std::vector<std::string> textureIds;
		uint32_t numVariables = 0;
	};

	static ShaderGraphType graphType;
	static std::string shaderVariableNamePrefix;
	static uint32_t numCurveInterpolationPoints;

	ShaderGraph();
	ShaderGraph(const std::unordered_map<uint32_t, ShaderNode>& initialNodes);

	std::string build(BuildResult& result, uint32_t nodeId = 0) const;

	uint32_t addNode(const std::string& typeName);
	void removeNode(uint32_t nodeId);
	void linkNodes(uint32_t sourceNodeId, uint32_t targetNodeId, uint32_t sourceSlot, uint32_t targetSlot);
	void linkNodes(uint32_t sourceNodeId, uint32_t targetNodeId, const std::string& sourceSlotName, const std::string& targetSlotName);
	void unlinkNodes(uint32_t sourceNodeId, uint32_t targetNodeId, uint32_t targetSlot);
	void unlinkNodes(uint32_t linkId);
	void setNodeParameter(uint32_t nodeId, uint32_t parameterIndex, ShaderParameter::Value value);
	void setNodeParameter(uint32_t nodeId, const std::string& parameterName, ShaderParameter::Value value);
	void setNodePosition(uint32_t nodeId, const vec2d& position);
	bool hasNode(uint32_t nodeId) const;
	const ShaderNode& getNode(uint32_t nodeId) const;
	const std::unordered_map<uint32_t, ShaderNode>& getNodes() const;
	uint32_t getNextNodeId() const;
	uint32_t getNextLinkId() const;

	bool hasNodeType(const std::string& typeName) const;
	const ShaderNodeType& getNodeType(const std::string& typeName) const;
	const ShaderNodeType& getNodeTypeOfNode(uint32_t nodeId) const;

private:
	uint32_t findNodeType(const std::string& typeName) const;
	uint32_t findNodeSignature(const BuildResult& result, const ShaderNode& node, std::vector<TypeMatch>& typeMatch) const;

	std::unordered_map<uint32_t, ShaderNode> nodes;
	uint32_t nextNodeId = 0;
	uint32_t nextLinkId = 0;
};

void to_json(nlohmann::ordered_json& j, const ShaderGraph& shader);
void from_json(const nlohmann::ordered_json& j, ShaderGraph& shader);
}