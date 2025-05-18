#pragma once

#include "../common/Types.h"
#include "../common/Math.h"
#include "../common/Id.h"
#include "../common/VariantParameter.h"
#include "ShaderNode.h"
#include "ShaderNodeType.h"
#include "ShaderGraphLanguage.h"
#include "../json/json.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>

namespace pixelpart {
class ShaderGraph {
public:
	using ShaderNodeCollection = std::unordered_map<id_t, ShaderNode>;

	class BuildException : public std::runtime_error {
	public:
		BuildException(const std::string& msg, id_t node = id_t(), std::uint32_t slot = id_t::nullValue);

		id_t node() const;
		std::uint32_t slot() const;

	private:
		id_t nodeId;
		std::uint32_t slotIndex = id_t::nullValue;
	};

	enum TypeMatch : std::uint32_t {
		typematch_exact,
		typematch_upcast,
		typematch_downcast,
		typematch_none
	};

	struct BuildResult {
		std::string mainCode;
		std::string parameterCode;
		std::unordered_map<id_t, std::vector<std::string>> nodeOutputVariables;
		std::unordered_map<id_t, std::uint32_t> nodeSignatures;
		std::unordered_set<id_t> resolvedNodes;
		std::vector<std::string> textureResourceIds;
		std::unordered_map<id_t, std::string> parameterNames;
		std::uint32_t variableCount = 0;
	};

	static ShaderGraphLanguage graphLanguage;
	static std::uint32_t curveInterpolationPointCount;

	ShaderGraph() = default;
	ShaderGraph(const ShaderNodeCollection& nodes);

	std::string build(BuildResult& result, id_t nodeId = id_t(0)) const;

	id_t addNode(const std::string& typeName);
	void removeNode(id_t nodeId);

	void linkNodes(id_t sourceNodeId, id_t targetNodeId, std::uint32_t sourceSlot, std::uint32_t targetSlot);
	void linkNodes(id_t sourceNodeId, id_t targetNodeId, const std::string& sourceSlotName, const std::string& targetSlotName);
	void unlinkNodes(id_t sourceNodeId, id_t targetNodeId, std::uint32_t targetSlot);
	void unlinkNodes(id_t linkId);

	void nodeName(id_t nodeId, const std::string& name);
	void nodeParameter(id_t nodeId, std::uint32_t parameterIndex, VariantParameter::Value value);
	void nodeParameter(id_t nodeId, const std::string& parameterName, VariantParameter::Value value);
	void nodeParameterNode(id_t nodeId, bool enable);
	void nodePosition(id_t nodeId, const float2_t& position);

	bool containsNode(id_t nodeId) const;
	const ShaderNode& node(id_t nodeId) const;
	const ShaderNodeCollection& nodes() const;

	std::unordered_map<id_t, VariantParameter> shaderParameters() const;

	bool containsNodeType(const std::string& typeName) const;
	const ShaderNodeType& nodeType(const std::string& typeName) const;
	const ShaderNodeType& nodeType(id_t nodeId) const;

private:
	std::uint32_t findNodeType(const std::string& typeName) const;
	std::uint32_t findNodeSignature(const BuildResult& result, const ShaderNode& node, std::vector<TypeMatch>& typeMatch) const;

	ShaderNodeCollection shaderNodes;
	id_t nextNodeId = id_t(0);
	id_t nextLinkId = id_t(0);
};

void to_json(nlohmann::ordered_json& j, const ShaderGraph& shader);
void from_json(const nlohmann::ordered_json& j, ShaderGraph& shader);
}