#pragma once

#include "ShaderNode.h"
#include "ShaderGraphLanguage.h"
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>

namespace pixelpart {
class ShaderGraph {
public:
	class BuildException : public std::runtime_error {
	public:
		BuildException(const std::string& msg, id_t node = nullId, uint32_t slot = nullId);

		id_t getNodeId() const;
		uint32_t getSlotIndex() const;

	private:
		id_t nodeId = nullId;
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
		std::string parameterCode;
		std::unordered_map<id_t, std::vector<std::string>> nodeOutputVariables;
		std::unordered_map<id_t, uint32_t> nodeSignatures;
		std::unordered_set<id_t> resolvedNodes;
		std::vector<std::string> textureResourceIds;
		std::vector<std::string> textureParameterNames;
		std::unordered_map<id_t, std::string> parameterNames;
		uint32_t numVariables = 0u;
	};

	static ShaderGraphLanguage graphLanguage;

	static uint32_t numCurveInterpolationPoints;

	ShaderGraph();
	ShaderGraph(const std::unordered_map<id_t, ShaderNode>& initialNodes);

	std::string build(BuildResult& result, id_t nodeId = 0u) const;

	id_t addNode(const std::string& typeName);
	void removeNode(id_t nodeId);
	void linkNodes(id_t sourceNodeId, id_t targetNodeId, uint32_t sourceSlot, uint32_t targetSlot);
	void linkNodes(id_t sourceNodeId, id_t targetNodeId, const std::string& sourceSlotName, const std::string& targetSlotName);
	void unlinkNodes(id_t sourceNodeId, id_t targetNodeId, uint32_t targetSlot);
	void unlinkNodes(id_t linkId);
	void setNodeName(id_t nodeId, const std::string& name);
	void setNodeParameter(id_t nodeId, uint32_t parameterIndex, VariantParameter::Value value);
	void setNodeParameter(id_t nodeId, const std::string& parameterName, VariantParameter::Value value);
	void setNodeParameterNode(id_t nodeId, bool enable);
	void setNodePosition(id_t nodeId, const vec2_t& position);
	bool hasNode(id_t nodeId) const;
	const ShaderNode& getNode(id_t nodeId) const;
	const std::unordered_map<id_t, ShaderNode>& getNodes() const;
	id_t getNextNodeId() const;
	id_t getNextLinkId() const;

	std::unordered_map<id_t, VariantParameter> getShaderParameters() const;

	bool hasNodeType(const std::string& typeName) const;
	const ShaderNodeType& getNodeType(const std::string& typeName) const;
	const ShaderNodeType& getNodeTypeOfNode(id_t nodeId) const;

private:
	uint32_t findNodeType(const std::string& typeName) const;
	uint32_t findNodeSignature(const BuildResult& result, const ShaderNode& node, std::vector<TypeMatch>& typeMatch) const;

	std::unordered_map<id_t, ShaderNode> nodes;

	id_t nextNodeId = 0u;
	id_t nextLinkId = 0u;
};

void to_json(nlohmann::ordered_json& j, const ShaderGraph& shader);
void from_json(const nlohmann::ordered_json& j, ShaderGraph& shader);
}