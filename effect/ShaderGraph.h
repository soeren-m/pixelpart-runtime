#pragma once

#include "../common/Types.h"
#include "../common/Id.h"
#include "../common/VariantParameter.h"
#include "ShaderNode.h"
#include "ShaderNodeType.h"
#include "ShaderGraphSpecification.h"
#include "../json/json.hpp"
#include <optional>
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
		BuildException(const char* msg, id_t node = id_t(), std::optional<std::uint32_t> slot = std::nullopt);
		BuildException(const std::string& msg, id_t node = id_t(), std::optional<std::uint32_t> slot = std::nullopt);

		id_t nodeId() const;
		std::optional<std::uint32_t> slotIndex() const;

	private:
		id_t exceptionNodeId;
		std::optional<std::uint32_t> exceptionSlotIndex;
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

	static ShaderGraphSpecification specification;

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
	enum class TypeMatch : std::uint32_t {
		exact,
		upcast,
		downcast,
		none
	};

	static constexpr std::int32_t floatPrecision = 6;
	static constexpr std::uint32_t curveInterpolationPointCount = 100;

	std::string buildValueString(const VariantValue& value, id_t nodeId, BuildResult& buildResult) const;
	std::string buildParameterValueString(const VariantParameter::Value& parameterValue, id_t nodeId, BuildResult& buildResult) const;

	std::optional<std::uint32_t> findNodeType(const std::string& typeName) const;
	std::optional<std::uint32_t> findNodeSignature(const ShaderNode& node, const BuildResult& buildResult, std::vector<TypeMatch>& typeMatch) const;

	ShaderNodeCollection graphNodes;
	id_t graphNextNodeId = id_t(0);
	id_t graphNextLinkId = id_t(0);
};

void to_json(nlohmann::ordered_json& j, const ShaderGraph& shader);
void from_json(const nlohmann::ordered_json& j, ShaderGraph& shader);
}
