#include "ShaderGraph.h"
#include "Serialization.h"
#include "StringFormat.h"
#include "SortedJson.h"
#include "../types/VariantValue.h"
#include <algorithm>

namespace pixelpart {
ShaderGraph::BuildException::BuildException(const char* msg, id_t node, std::optional<std::uint32_t> slot) :
	std::runtime_error(msg), exceptionNodeId(node), exceptionSlotIndex(slot) {

}
ShaderGraph::BuildException::BuildException(const std::string& msg, id_t node, std::optional<std::uint32_t> slot) :
	std::runtime_error(msg), exceptionNodeId(node), exceptionSlotIndex(slot) {

}

id_t ShaderGraph::BuildException::nodeId() const {
	return exceptionNodeId;
}
std::optional<std::uint32_t> ShaderGraph::BuildException::slotIndex() const {
	return exceptionSlotIndex;
}

ShaderGraphSpecification ShaderGraph::specification = ShaderGraphSpecification();

ShaderGraph::ShaderGraph(const ShaderNodeCollection& nodes) : graphNodes(nodes) {
	id_t maxNodeId = 0;
	id_t maxLinkId = 0;

	for(const auto& [nodeId, node] : graphNodes) {
		maxNodeId = std::max(maxNodeId, nodeId);

		for(const auto& link : node.inputs()) {
			if(!link.id) {
				continue;
			}

			maxLinkId = std::max(maxLinkId.value(), link.id.value());
		}
	}

	graphNextNodeId = maxNodeId.value() + 1;
	graphNextLinkId = maxLinkId.value() + 1;
}

std::string ShaderGraph::build(BuildResult& result, id_t nodeId) const {
	if(nodeId.value() == 0) {
		result = BuildResult();
	}

	auto nodeIt = graphNodes.find(nodeId);
	if(nodeIt == graphNodes.end()) {
		return std::string();
	}

	const ShaderNode& node = nodeIt->second;

	std::optional<std::uint32_t> nodeTypeIndex = findNodeType(node.type());
	if(!nodeTypeIndex) {
		throw BuildException("Unknown node type \"" + node.type() + "\"", nodeId);
	}

	const ShaderNodeType& shaderNodeType = specification.nodes.at(nodeTypeIndex.value());

	std::string code = shaderNodeType.code;

	for(std::size_t parameterIndex = 0; parameterIndex < node.parameters().size(); parameterIndex++) {
		const VariantParameter::Value& parameterValue = node.parameters().at(parameterIndex);
		std::string valueString;

		if(node.parameterNode()) {
			if(parameterValue.type() == VariantParameter::Value::type_resource_image) {
				std::size_t samplerIndex = result.textureResourceIds.size();
				if(samplerIndex < specification.textureSamplers.size()) {
					result.textureResourceIds.push_back(parameterValue.valueResourceId());
					result.parameterNames[nodeId] = specification.textureSamplers[samplerIndex];

					valueString = specification.textureSamplers[samplerIndex];
				}
				else {
					throw BuildException("No more texture samplers available", nodeId);
				}
			}
			else {
				std::string nodeIdentifier = replaceString(shaderNodeType.name, "_", " ");
				std::string parameterVariableName = specification.parameterPrefix
					+ nodeIdentifier
					+ "_" + serializeInt(nodeId.value())
					+ "_" + serializeInt(parameterIndex);

				result.parameterNames[nodeId] = parameterVariableName;

				std::string parameterCode = specification.parameterDefinition;
				parameterCode = replaceString(parameterCode, specification.parameterTypeNames.at(parameterValue.type()), "{type}");
				parameterCode = replaceString(parameterCode, parameterVariableName, "{name}");
				result.parameterCode += parameterCode;
				result.parameterCode += "\n";

				valueString = parameterVariableName;
			}
		}
		else {
			valueString = buildParameterValueString(parameterValue, nodeId, result);
		}

		code = replaceString(code, valueString, "{param" + serializeInt(parameterIndex) + "}");
	}

	std::string inputCode;
	for(std::size_t inputSlot = 0; inputSlot < shaderNodeType.inputs.size(); inputSlot++) {
		ShaderNode::Link link = inputSlot < node.inputs().size()
			? node.inputs()[inputSlot]
			: ShaderNode::Link();

		if(link.id && graphNodes.count(link.nodeId) != 0) {
			if(result.resolvedNodes.count(link.nodeId) == 0) {
				inputCode += build(result, link.nodeId);
			}
		}
		else if(shaderNodeType.defaultInputs[inputSlot].type() != VariantValue::type_null) {
			std::string defaultValueString = buildValueString(shaderNodeType.defaultInputs[inputSlot], nodeId, result);

			code = replaceString(code,
				defaultValueString,
				"{in" + serializeInt(inputSlot) + "}");
		}
		else {
			throw BuildException("Input node not found", nodeId, static_cast<std::uint32_t>(inputSlot));
		}
	}

	std::vector<TypeMatch> typeMatch;
	std::optional<std::uint32_t> signatureIndex = findNodeSignature(node, result, typeMatch);
	if(!signatureIndex) {
		throw BuildException("No matching node signature found", nodeId);
	}

	result.nodeSignatures[nodeId] = signatureIndex.value();

	const ShaderNodeType::Signature& signature = shaderNodeType.signatures[result.nodeSignatures[nodeId]];

	result.nodeOutputVariables[nodeId] = std::vector<std::string>();
	for(std::size_t outputSlot = 0; outputSlot < shaderNodeType.outputs.size(); outputSlot++) {
		std::string outputVariableName = specification.variablePrefix + serializeInt(result.variableCount);
		std::string outputVariableDefinition = specification.typeNames[static_cast<std::size_t>(signature.outputTypes[outputSlot])] + " " + outputVariableName;
		result.nodeOutputVariables[nodeId].push_back(outputVariableName);

		code = replaceString(code, outputVariableName, "{out" + serializeInt(outputSlot) + "}");
		code = replaceString(code, outputVariableDefinition, "{outdef" + serializeInt(outputSlot) + "}");
		result.variableCount++;
	}

	for(std::size_t inputSlot = 0; inputSlot < node.inputs().size(); inputSlot++) {
		const ShaderNode::Link& link = node.inputs()[inputSlot];
		if(typeMatch[inputSlot] == TypeMatch::none) {
			throw BuildException("Types do not match", nodeId, static_cast<std::uint32_t>(inputSlot));
		}

		VariantValue::Type sourceValueType = VariantValue::type_null;

		if(graphNodes.count(link.nodeId) != 0) {
			const ShaderNodeType& sourceNodeType = nodeType(link.nodeId);
			const ShaderNodeType::Signature& sourceNodeSignature = sourceNodeType.signatures[result.nodeSignatures[link.nodeId]];

			sourceValueType = sourceNodeSignature.outputTypes[link.slot];
		}
		else {
			sourceValueType = shaderNodeType.defaultInputs[inputSlot].type();
		}

		std::string inputVariableTemplate = "{in" + serializeInt(inputSlot) + "}";

		if(typeMatch[inputSlot] != TypeMatch::exact) {
			std::string codeTypeCast = specification.typeCasts[signature.inputTypes[inputSlot]][sourceValueType];
			codeTypeCast = replaceString(codeTypeCast, inputVariableTemplate, "{0}");
			code = replaceString(code, codeTypeCast, inputVariableTemplate);
		}

		code = replaceString(code, result.nodeOutputVariables[link.nodeId][link.slot], inputVariableTemplate);
	}

	result.resolvedNodes.insert(nodeId);

	if(nodeId.value() == 0) {
		result.mainCode = inputCode + "\n" + code;

		return result.mainCode;
	}

	return inputCode + "\n" + code;
}

id_t ShaderGraph::addNode(const std::string& typeName) {
	std::optional<std::uint32_t> typeIndex = findNodeType(typeName);
	if(!typeIndex) {
		return id_t();
	}

	id_t nodeId = graphNextNodeId++;
	graphNodes[nodeId] = ShaderNode(specification.nodes[typeIndex.value()]);

	return nodeId;
}
void ShaderGraph::removeNode(id_t nodeId) {
	for(auto& [otherNodeId, otherNode] : graphNodes) {
		for(auto& link : otherNode.inputs()) {
			if(link.nodeId == nodeId) {
				link = ShaderNode::Link();
			}
		}
	}

	graphNodes.erase(nodeId);
}

void ShaderGraph::linkNodes(id_t sourceNodeId, id_t targetNodeId, std::uint32_t sourceSlot, std::uint32_t targetSlot) {
	if(!containsNode(sourceNodeId) || !containsNode(targetNodeId)) {
		return;
	}

	ShaderNode& sourceNode = graphNodes[sourceNodeId];
	ShaderNode& targetNode = graphNodes[targetNodeId];
	if(targetSlot >= targetNode.inputs().size() || sourceSlot >= nodeType(sourceNode.type()).outputs.size()) {
		return;
	}

	targetNode.inputs()[targetSlot] = ShaderNode::Link(
		graphNextLinkId++,
		sourceNodeId,
		sourceSlot);
}
void ShaderGraph::linkNodes(id_t sourceNodeId, id_t targetNodeId, const std::string& sourceSlotName, const std::string& targetSlotName) {
	if(!containsNode(sourceNodeId) || !containsNode(targetNodeId)) {
		return;
	}

	ShaderNode& sourceNode = graphNodes[sourceNodeId];
	ShaderNode& targetNode = graphNodes[targetNodeId];
	const ShaderNodeType& sourceNodeType = nodeType(sourceNode.type());
	const ShaderNodeType& targetNodeType = nodeType(targetNode.type());

	std::optional<std::uint32_t> sourceSlot;
	for(std::size_t outputSlot = 0; outputSlot < sourceNodeType.outputs.size(); outputSlot++) {
		if(sourceNodeType.outputs[outputSlot] == sourceSlotName) {
			sourceSlot = static_cast<std::uint32_t>(outputSlot);
		}
	}

	std::optional<std::uint32_t> targetSlot;
	for(std::size_t inputSlot = 0; inputSlot < targetNodeType.inputs.size(); inputSlot++) {
		if(targetNodeType.inputs[inputSlot] == targetSlotName) {
			targetSlot = static_cast<std::uint32_t>(inputSlot);
		}
	}

	if(sourceSlot && targetSlot) {
		targetNode.inputs()[targetSlot.value()] = ShaderNode::Link(
			graphNextLinkId++,
			sourceNodeId,
			sourceSlot.value());
	}
}
void ShaderGraph::unlinkNodes(id_t sourceNodeId, id_t targetNodeId, std::uint32_t targetSlot) {
	if(!containsNode(sourceNodeId) || !containsNode(targetNodeId)) {
		return;
	}

	ShaderNode& targetNode = graphNodes[targetNodeId];
	if(targetSlot >= targetNode.inputs().size()) {
		return;
	}

	targetNode.inputs()[targetSlot] = ShaderNode::Link();
}
void ShaderGraph::unlinkNodes(id_t linkId) {
	for(auto& [nodeId, node] : graphNodes) {
		for(auto& link : node.inputs()) {
			if(link.id == linkId) {
				link = ShaderNode::Link();
				return;
			}
		}
	}
}

void ShaderGraph::nodeName(id_t nodeId, const std::string& name) {
	if(!containsNode(nodeId)) {
		return;
	}

	graphNodes[nodeId].name(name);
}
void ShaderGraph::nodeParameter(id_t nodeId, std::uint32_t parameterIndex, VariantParameter::Value value) {
	if(!containsNode(nodeId) || parameterIndex >= graphNodes[nodeId].parameters().size()) {
		return;
	}

	graphNodes[nodeId].parameters()[parameterIndex] = value;
}
void ShaderGraph::nodeParameter(id_t nodeId, const std::string& parameterName, VariantParameter::Value value) {
	if(!containsNode(nodeId)) {
		return;
	}

	ShaderNode& node = graphNodes[nodeId];
	for(std::size_t parameterIndex = 0; parameterIndex < graphNodes[nodeId].parameters().size(); parameterIndex++) {
		if(nodeType(node.type()).parameters[parameterIndex].name() == parameterName) {
			node.parameters()[parameterIndex] = value;
		}
	}
}
void ShaderGraph::nodeParameterNode(id_t nodeId, bool enable) {
	if(!containsNode(nodeId)) {
		return;
	}

	graphNodes[nodeId].parameterNode(enable);
}
void ShaderGraph::nodePosition(id_t nodeId, const float2_t& position) {
	if(!containsNode(nodeId)) {
		return;
	}

	graphNodes[nodeId].move(position);
}

bool ShaderGraph::containsNode(id_t nodeId) const {
	return graphNodes.count(nodeId) != 0;
}
const ShaderNode& ShaderGraph::node(id_t nodeId) const {
	return graphNodes.at(nodeId);
}
const ShaderGraph::ShaderNodeCollection& ShaderGraph::nodes() const {
	return graphNodes;
}

std::unordered_map<id_t, VariantParameter> ShaderGraph::shaderParameters() const {
	std::unordered_map<id_t, VariantParameter> parameters;

	for(const auto& [nodeId, node] : graphNodes) {
		const ShaderNodeType& shaderNodeType = nodeType(node.type());

		if(node.parameterNode() && !shaderNodeType.parameters.empty()) {
			parameters[nodeId] = shaderNodeType.parameters[0];
		}
	}

	return parameters;
}

bool ShaderGraph::containsNodeType(const std::string& typeName) const{
	return findNodeType(typeName).has_value();
}
const ShaderNodeType& ShaderGraph::nodeType(const std::string& typeName) const {
	return specification.nodes.at(findNodeType(typeName).value());
}
const ShaderNodeType& ShaderGraph::nodeType(id_t nodeId) const {
	return specification.nodes.at(findNodeType(graphNodes.at(nodeId).type()).value());
}

std::string ShaderGraph::buildValueString(const VariantValue& value, id_t nodeId, BuildResult& buildResult) const {
	std::string valueString;
	switch(value.type()) {
		case VariantValue::type_bool: {
			valueString = specification.typeConstructors[VariantValue::type_bool];
			valueString = replaceString(valueString, value.valueBool() ? "true" : "false", "{0}");
			break;
		}
		case VariantValue::type_int: {
			valueString = specification.typeConstructors[VariantValue::type_int];
			valueString = replaceString(valueString, serializeInt(value.valueInt()), "{0}");
			break;
		}
		case VariantValue::type_float: {
			valueString = specification.typeConstructors[VariantValue::type_float];
			valueString = replaceString(valueString, serializeFloat(value.valueFloat(), FloatFormat::fixed, floatPrecision), "{0}");
			break;
		}
		case VariantValue::type_float2: {
			valueString = specification.typeConstructors[VariantValue::type_float2];
			valueString = replaceString(valueString, serializeFloat(value.valueFloat2().x, FloatFormat::fixed, floatPrecision), "{0}");
			valueString = replaceString(valueString, serializeFloat(value.valueFloat2().y, FloatFormat::fixed, floatPrecision), "{1}");
			break;
		}
		case VariantValue::type_float3: {
			valueString = specification.typeConstructors[VariantValue::type_float3];
			valueString = replaceString(valueString, serializeFloat(value.valueFloat3().x, FloatFormat::fixed, floatPrecision), "{0}");
			valueString = replaceString(valueString, serializeFloat(value.valueFloat3().y, FloatFormat::fixed, floatPrecision), "{1}");
			valueString = replaceString(valueString, serializeFloat(value.valueFloat3().z, FloatFormat::fixed, floatPrecision), "{2}");
			break;
		}
		case VariantValue::type_float4: {
			valueString = specification.typeConstructors[VariantValue::type_float4];
			valueString = replaceString(valueString, serializeFloat(value.valueFloat4().x, FloatFormat::fixed, floatPrecision), "{0}");
			valueString = replaceString(valueString, serializeFloat(value.valueFloat4().y, FloatFormat::fixed, floatPrecision), "{1}");
			valueString = replaceString(valueString, serializeFloat(value.valueFloat4().z, FloatFormat::fixed, floatPrecision), "{2}");
			valueString = replaceString(valueString, serializeFloat(value.valueFloat4().w, FloatFormat::fixed, floatPrecision), "{3}");
			break;
		}
		default: {
			break;
		}
	}

	return valueString;
}
std::string ShaderGraph::buildParameterValueString(const VariantParameter::Value& parameterValue, id_t nodeId, BuildResult& buildResult) const {
	std::string valueString;
	switch(parameterValue.type()) {
		case VariantParameter::Value::type_int:
		case VariantParameter::Value::type_enum: {
			valueString = specification.typeConstructors[VariantValue::type_int];
			valueString = replaceString(valueString, serializeInt(parameterValue.valueInt()), "{0}");
			break;
		}
		case VariantParameter::Value::type_float: {
			valueString = specification.typeConstructors[VariantValue::type_float];
			valueString = replaceString(valueString, serializeFloat(parameterValue.valueFloat(), FloatFormat::fixed, floatPrecision), "{0}");
			break;
		}
		case VariantParameter::Value::type_float2: {
			valueString = specification.typeConstructors[VariantValue::type_float2];
			valueString = replaceString(valueString, serializeFloat(parameterValue.valueFloat2().x, FloatFormat::fixed, floatPrecision), "{0}");
			valueString = replaceString(valueString, serializeFloat(parameterValue.valueFloat2().y, FloatFormat::fixed, floatPrecision), "{1}");
			break;
		}
		case VariantParameter::Value::type_float3: {
			valueString = specification.typeConstructors[VariantValue::type_float3];
			valueString = replaceString(valueString, serializeFloat(parameterValue.valueFloat3().x, FloatFormat::fixed, floatPrecision), "{0}");
			valueString = replaceString(valueString, serializeFloat(parameterValue.valueFloat3().y, FloatFormat::fixed, floatPrecision), "{1}");
			valueString = replaceString(valueString, serializeFloat(parameterValue.valueFloat3().z, FloatFormat::fixed, floatPrecision), "{2}");
			break;
		}
		case VariantParameter::Value::type_float4:
		case VariantParameter::Value::type_color: {
			valueString = specification.typeConstructors[VariantValue::type_float4];
			valueString = replaceString(valueString, serializeFloat(parameterValue.valueFloat4().x, FloatFormat::fixed, floatPrecision), "{0}");
			valueString = replaceString(valueString, serializeFloat(parameterValue.valueFloat4().y, FloatFormat::fixed, floatPrecision), "{1}");
			valueString = replaceString(valueString, serializeFloat(parameterValue.valueFloat4().z, FloatFormat::fixed, floatPrecision), "{2}");
			valueString = replaceString(valueString, serializeFloat(parameterValue.valueFloat4().w, FloatFormat::fixed, floatPrecision), "{3}");
			break;
		}
		case VariantParameter::Value::type_bool: {
			valueString = specification.typeConstructors[VariantValue::type_bool];
			valueString = replaceString(valueString, parameterValue.valueBool() ? "true" : "false", "{0}");
			break;
		}
		case VariantParameter::Value::type_curve: {
			Curve<float_t> curve = parameterValue.valueCurve();

			for(std::uint32_t pointIndex = 0; pointIndex < curveInterpolationPointCount; pointIndex++) {
				float_t pointValue = curve.at(static_cast<float_t>(pointIndex) / static_cast<float_t>(curveInterpolationPointCount - 1));
				std::string pointValueString = specification.typeConstructors[VariantValue::type_float];
				pointValueString = replaceString(pointValueString, serializeFloat(pointValue, FloatFormat::fixed, floatPrecision), "{0}");

				valueString += pointValueString;
				if(pointIndex + 1 < curveInterpolationPointCount) {
					valueString += ", ";
				}
			}

			break;
		}
		case VariantParameter::Value::type_gradient: {
			Curve<float3_t> gradient = parameterValue.valueGradient();

			for(std::uint32_t pointIndex = 0; pointIndex < curveInterpolationPointCount; pointIndex++) {
				float3_t pointValue = gradient.at(static_cast<float_t>(pointIndex) / static_cast<float_t>(curveInterpolationPointCount - 1));
				std::string pointValueString = specification.typeConstructors[VariantValue::type_float3];
				pointValueString = replaceString(pointValueString, serializeFloat(pointValue.x, FloatFormat::fixed, floatPrecision), "{0}");
				pointValueString = replaceString(pointValueString, serializeFloat(pointValue.y, FloatFormat::fixed, floatPrecision), "{1}");
				pointValueString = replaceString(pointValueString, serializeFloat(pointValue.z, FloatFormat::fixed, floatPrecision), "{2}");

				valueString += pointValueString;
				if(pointIndex + 1 < curveInterpolationPointCount) {
					valueString += ", ";
				}
			}

			break;
		}
		case VariantParameter::Value::type_resource_image: {
			std::size_t samplerIndex = buildResult.textureResourceIds.size();
			if(samplerIndex < specification.textureSamplers.size()) {
				buildResult.textureResourceIds.push_back(parameterValue.valueResourceId());
				valueString = specification.textureSamplers[samplerIndex];
			}
			else {
				throw BuildException("No more texture samplers available", nodeId);
			}

			break;
		}
		default: {
			break;
		}
	}

	return valueString;
}

std::optional<std::uint32_t> ShaderGraph::findNodeType(const std::string& typeName) const {
	for(std::size_t nodeIndex = 0; nodeIndex < specification.nodes.size(); nodeIndex++) {
		if(specification.nodes[nodeIndex].name == typeName) {
			return static_cast<std::uint32_t>(nodeIndex);
		}
	}

	return std::nullopt;
}
std::optional<std::uint32_t> ShaderGraph::findNodeSignature(const ShaderNode& node, const BuildResult& buildResult, std::vector<TypeMatch>& typeMatch) const {
	const auto matchTypes = [](VariantValue::Type type1, VariantValue::Type type2) {
		if(type1 == VariantValue::type_null || type2 == VariantValue::type_null) {
			return ShaderGraph::TypeMatch::none;
		}
		else if(type1 == type2) {
			return ShaderGraph::TypeMatch::exact;
		}
		else if(type1 > type2) {
			return ShaderGraph::TypeMatch::downcast;
		}
		else if(type1 < type2 && (type1 == VariantValue::type_bool || type1 == VariantValue::type_int || type1 == VariantValue::type_float)) {
			return ShaderGraph::TypeMatch::upcast;
		}

		return ShaderGraph::TypeMatch::none;
	};

	const ShaderNodeType& shaderNodeType = nodeType(node.type());

	std::optional<std::uint32_t> bestSignature;
	typeMatch = std::vector<TypeMatch>(shaderNodeType.inputs.size(), TypeMatch::none);

	for(std::size_t signatureIndex = 0; signatureIndex < shaderNodeType.signatures.size(); signatureIndex++) {
		const ShaderNodeType::Signature& signature = shaderNodeType.signatures[signatureIndex];

		std::vector<TypeMatch> currentMatch(node.inputs().size(), TypeMatch::none);
		for(std::size_t inputIndex = 0; inputIndex < node.inputs().size(); inputIndex++) {
			VariantValue::Type inputType = VariantValue::type_null;
			id_t sourceNodeId = node.inputs()[inputIndex].nodeId;

			if(graphNodes.count(sourceNodeId) != 0) {
				std::uint32_t sourceSlot = node.inputs()[inputIndex].slot;
				std::uint32_t sourceSignature = buildResult.nodeSignatures.at(sourceNodeId);
				const ShaderNodeType& sourceNodeType = nodeType(sourceNodeId);

				inputType = sourceNodeType.signatures[sourceSignature].outputTypes[sourceSlot];
			}
			else {
				inputType = shaderNodeType.defaultInputs[inputIndex].type();
			}

			currentMatch[inputIndex] = matchTypes(inputType, signature.inputTypes[inputIndex]);
		}

		bool matchNotWorse = true;
		bool matchBetterForAnyInput = node.inputs().empty();
		for(std::size_t inputIndex = 0; inputIndex < node.inputs().size(); inputIndex++) {
			if(currentMatch[inputIndex] == TypeMatch::none || currentMatch[inputIndex] > typeMatch[inputIndex]) {
				matchNotWorse = false;
				break;
			}
			else if(currentMatch[inputIndex] < typeMatch[inputIndex]) {
				matchBetterForAnyInput = true;
			}
		}

		if(matchNotWorse && matchBetterForAnyInput) {
			typeMatch = currentMatch;
			bestSignature = static_cast<std::uint32_t>(signatureIndex);
		}
	}

	return bestSignature;
}

void to_json(nlohmann::ordered_json& j, const ShaderGraph& shader) {
	j = nlohmann::ordered_json{
		{ "nodes", toSortedJson(shader.nodes()) }
	};
}
void from_json(const nlohmann::ordered_json& j, ShaderGraph& shader) {
	shader = ShaderGraph(j.at("nodes").get<ShaderGraph::ShaderNodeCollection>());
}
}
