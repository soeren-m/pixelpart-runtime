#include "ShaderGraph.h"
#include "../common/VariantValue.h"
#include "../common/Serialization.h"
#include "../common/StringFormat.h"
#include "../common/SortedJson.h"
#include <algorithm>

namespace pixelpart {
ShaderGraph::BuildException::BuildException(const std::string& msg, id_t node, std::uint32_t slot) :
	std::runtime_error(msg), nodeId(node), slotIndex(slot) {

}

id_t ShaderGraph::BuildException::node() const {
	return nodeId;
}
std::uint32_t ShaderGraph::BuildException::slot() const {
	return slotIndex;
}

ShaderGraphLanguage ShaderGraph::graphLanguage = ShaderGraphLanguage();
std::uint32_t ShaderGraph::curveInterpolationPointCount = 100;

ShaderGraph::ShaderGraph(const ShaderNodeCollection& nodes) : shaderNodes(nodes) {
	id_t maxNodeId = 0;
	id_t maxLinkId = 0;
	for(const auto& nodeEntry : shaderNodes) {
		maxNodeId = std::max(maxNodeId, nodeEntry.first);

		for(const auto& link : nodeEntry.second.inputs()) {
			if(!link.id) {
				continue;
			}

			maxLinkId = std::max(maxLinkId.value(), link.id.value());
		}
	}

	nextNodeId = maxNodeId.value() + 1;
	nextLinkId = maxLinkId.value() + 1;
}

std::string ShaderGraph::build(BuildResult& result, id_t nodeId) const {
	if(nodeId.value() == 0) {
		result = BuildResult();
	}

	auto nodeEntry = shaderNodes.find(nodeId);
	if(nodeEntry == shaderNodes.end()) {
		return std::string();
	}

	const ShaderNode& node = nodeEntry->second;

	std::uint32_t nodeTypeIndex = findNodeType(node.type());
	if(nodeTypeIndex == id_t::nullValue) {
		throw BuildException("Unknown node type \"" + node.type() + "\"", nodeId);
	}

	const ShaderNodeType& shaderNodeType = graphLanguage.nodes.at(nodeTypeIndex);

	std::string code = shaderNodeType.code;

	for(std::size_t parameterIndex = 0; parameterIndex < node.parameters().size(); parameterIndex++) {
		const VariantParameter::Value& parameterValue = node.parameters().at(parameterIndex);
		std::string valueString;

		if(node.parameterNode()) {
			if(parameterValue.type() == VariantParameter::Value::type_resource_image) {
				std::size_t samplerIndex = result.textureResourceIds.size();
				if(samplerIndex < graphLanguage.textureSamplers.size()) {
					result.textureResourceIds.push_back(parameterValue.valueResourceId());
					result.parameterNames[nodeId] = graphLanguage.textureSamplers[samplerIndex];

					valueString = graphLanguage.textureSamplers[samplerIndex];
				}
				else {
					throw BuildException("No more texture samplers available", nodeId);
				}
			}
			else {
				std::string nodeIdentifier = replaceString(shaderNodeType.name, "_", " ");
				std::string parameterVariableName = graphLanguage.parameterPrefix
					+ nodeIdentifier
					+ "_" + serializeInt(nodeId.value())
					+ "_" + serializeInt(parameterIndex);

				result.parameterNames[nodeId] = parameterVariableName;

				std::string parameterCode = graphLanguage.parameterDefinition;
				parameterCode = replaceString(parameterCode, graphLanguage.parameterTypeNames.at(parameterValue.type()), "{type}");
				parameterCode = replaceString(parameterCode, parameterVariableName, "{name}");
				result.parameterCode += parameterCode;
				result.parameterCode += "\n";

				valueString = parameterVariableName;
			}
		}
		else {
			switch(parameterValue.type()) {
				case VariantParameter::Value::type_int:
				case VariantParameter::Value::type_enum: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_int];
					valueString = replaceString(valueString, serializeInt(parameterValue.valueInt()), "{0}");
					break;
				}
				case VariantParameter::Value::type_float: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_float];
					valueString = replaceString(valueString, serializeFloat(parameterValue.valueFloat(), 6), "{0}");
					break;
				}
				case VariantParameter::Value::type_float2: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_float2];
					valueString = replaceString(valueString, serializeFloat(parameterValue.valueFloat2().x, 6), "{0}");
					valueString = replaceString(valueString, serializeFloat(parameterValue.valueFloat2().y, 6), "{1}");
					break;
				}
				case VariantParameter::Value::type_float3: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_float3];
					valueString = replaceString(valueString, serializeFloat(parameterValue.valueFloat3().x, 6), "{0}");
					valueString = replaceString(valueString, serializeFloat(parameterValue.valueFloat3().y, 6), "{1}");
					valueString = replaceString(valueString, serializeFloat(parameterValue.valueFloat3().z, 6), "{2}");
					break;
				}
				case VariantParameter::Value::type_float4:
				case VariantParameter::Value::type_color: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_float4];
					valueString = replaceString(valueString, serializeFloat(parameterValue.valueFloat4().x, 6), "{0}");
					valueString = replaceString(valueString, serializeFloat(parameterValue.valueFloat4().y, 6), "{1}");
					valueString = replaceString(valueString, serializeFloat(parameterValue.valueFloat4().z, 6), "{2}");
					valueString = replaceString(valueString, serializeFloat(parameterValue.valueFloat4().w, 6), "{3}");
					break;
				}
				case VariantParameter::Value::type_bool: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_bool];
					valueString = replaceString(valueString, parameterValue.valueBool() ? "true" : "false", "{0}");
					break;
				}
				case VariantParameter::Value::type_curve: {
					Curve<float_t> curve = parameterValue.valueCurve();

					for(std::uint32_t pointIndex = 0; pointIndex < curveInterpolationPointCount; pointIndex++) {
						float_t pointValue = curve.at(static_cast<float_t>(pointIndex) / static_cast<float_t>(curveInterpolationPointCount - 1));
						std::string pointValueString = graphLanguage.typeConstructors[VariantValue::type_float];
						pointValueString = replaceString(pointValueString, serializeFloat(pointValue, 6), "{0}");

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
						std::string pointValueString = graphLanguage.typeConstructors[VariantValue::type_float3];
						pointValueString = replaceString(pointValueString, serializeFloat(pointValue.x, 6), "{0}");
						pointValueString = replaceString(pointValueString, serializeFloat(pointValue.y, 6), "{1}");
						pointValueString = replaceString(pointValueString, serializeFloat(pointValue.z, 6), "{2}");

						valueString += pointValueString;
						if(pointIndex + 1 < curveInterpolationPointCount) {
							valueString += ", ";
						}
					}

					break;
				}
				case VariantParameter::Value::type_resource_image: {
					std::size_t samplerIndex = result.textureResourceIds.size();
					if(samplerIndex < graphLanguage.textureSamplers.size()) {
						result.textureResourceIds.push_back(parameterValue.valueResourceId());
						valueString = graphLanguage.textureSamplers[samplerIndex];
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
		}

		code = replaceString(code, valueString, "{param" + serializeInt(parameterIndex) + "}");
	}

	std::string inputCode;
	for(std::size_t inputSlot = 0; inputSlot < shaderNodeType.inputs.size(); inputSlot++) {
		ShaderNode::Link link;
		if(inputSlot < node.inputs().size()) {
			link = node.inputs()[inputSlot];
		}

		if(shaderNodes.count(link.nodeId) != 0) {
			if(result.resolvedNodes.count(link.nodeId) == 0) {
				inputCode += build(result, link.nodeId);
			}
		}
		else if(shaderNodeType.defaultInputs[inputSlot].type() != VariantValue::type_null) {
			std::string valueString;
			switch(shaderNodeType.defaultInputs[inputSlot].type()) {
				case VariantValue::type_bool: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_bool];
					valueString = replaceString(valueString, shaderNodeType.defaultInputs[inputSlot].valueBool() ? "true" : "false", "{0}");
					break;
				}
				case VariantValue::type_int: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_int];
					valueString = replaceString(valueString, serializeInt(shaderNodeType.defaultInputs[inputSlot].valueInt()), "{0}");
					break;
				}
				case VariantValue::type_float: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_float];
					valueString = replaceString(valueString, serializeFloat(shaderNodeType.defaultInputs[inputSlot].valueFloat(), 6), "{0}");
					break;
				}
				case VariantValue::type_float2: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_float2];
					valueString = replaceString(valueString, serializeFloat(shaderNodeType.defaultInputs[inputSlot].valueFloat2().x, 6), "{0}");
					valueString = replaceString(valueString, serializeFloat(shaderNodeType.defaultInputs[inputSlot].valueFloat2().y, 6), "{1}");
					break;
				}
				case VariantValue::type_float3: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_float3];
					valueString = replaceString(valueString, serializeFloat(shaderNodeType.defaultInputs[inputSlot].valueFloat3().x, 6), "{0}");
					valueString = replaceString(valueString, serializeFloat(shaderNodeType.defaultInputs[inputSlot].valueFloat3().y, 6), "{1}");
					valueString = replaceString(valueString, serializeFloat(shaderNodeType.defaultInputs[inputSlot].valueFloat3().z, 6), "{2}");
					break;
				}
				case VariantValue::type_float4: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_float4];
					valueString = replaceString(valueString, serializeFloat(shaderNodeType.defaultInputs[inputSlot].valueFloat4().x, 6), "{0}");
					valueString = replaceString(valueString, serializeFloat(shaderNodeType.defaultInputs[inputSlot].valueFloat4().y, 6), "{1}");
					valueString = replaceString(valueString, serializeFloat(shaderNodeType.defaultInputs[inputSlot].valueFloat4().z, 6), "{2}");
					valueString = replaceString(valueString, serializeFloat(shaderNodeType.defaultInputs[inputSlot].valueFloat4().w, 6), "{3}");
					break;
				}
				default: {
					break;
				}
			}

			code = replaceString(code, valueString, "{in" + serializeInt(inputSlot) + "}");
		}
		else {
			throw BuildException("Input node not found", nodeId, static_cast<std::uint32_t>(inputSlot));
		}
	}

	std::vector<TypeMatch> typeMatch;
	result.nodeSignatures[nodeId] = findNodeSignature(result, node, typeMatch);
	if(result.nodeSignatures[nodeId] == id_t::nullValue) {
		throw BuildException("No matching node signature found", nodeId);
	}

	const ShaderNodeType::Signature& signature = shaderNodeType.signatures[result.nodeSignatures[nodeId]];

	result.nodeOutputVariables[nodeId] = std::vector<std::string>();
	for(std::size_t outputSlot = 0; outputSlot < shaderNodeType.outputs.size(); outputSlot++) {
		std::string outputVariableName = graphLanguage.variablePrefix + serializeInt(result.variableCount++);
		std::string outputVariableDefinition = graphLanguage.typeNames[static_cast<std::size_t>(signature.outputTypes[outputSlot])] + " " + outputVariableName;
		result.nodeOutputVariables[nodeId].push_back(outputVariableName);

		code = replaceString(code, outputVariableName, "{out" + serializeInt(outputSlot) + "}");
		code = replaceString(code, outputVariableDefinition, "{outdef" + serializeInt(outputSlot) + "}");
		result.variableCount++;
	}

	for(std::size_t inputSlot = 0; inputSlot < node.inputs().size(); inputSlot++) {
		const ShaderNode::Link& link = node.inputs()[inputSlot];
		if(typeMatch[inputSlot] == typematch_none) {
			throw BuildException("Types do not match", nodeId, inputSlot);
		}

		VariantValue::Type sourceValueType = VariantValue::type_null;

		if(shaderNodes.count(link.nodeId) != 0) {
			const ShaderNodeType& sourceNodeType = nodeType(link.nodeId);
			const ShaderNodeType::Signature& sourceNodeSignature = sourceNodeType.signatures[result.nodeSignatures[link.nodeId]];

			sourceValueType = sourceNodeSignature.outputTypes[link.slot];
		}
		else {
			sourceValueType = shaderNodeType.defaultInputs[inputSlot].type();
		}

		std::string inputVariableTemplate = "{in" + serializeInt(inputSlot) + "}";

		if(typeMatch[inputSlot] != typematch_exact) {
			std::string codeTypeCast = graphLanguage.typeCasts[signature.inputTypes[inputSlot]][sourceValueType];
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
	std::uint32_t typeIndex = findNodeType(typeName);
	if(typeIndex == id_t::nullValue) {
		return id_t();
	}

	id_t nodeId = nextNodeId++;
	shaderNodes[nodeId] = ShaderNode(graphLanguage.nodes[typeIndex]);

	return nodeId;
}
void ShaderGraph::removeNode(id_t nodeId) {
	for(auto& nodeEntry : shaderNodes) {
		for(auto& link : nodeEntry.second.inputs()) {
			if(link.nodeId == nodeId) {
				link = ShaderNode::Link();
			}
		}
	}

	shaderNodes.erase(nodeId);
}

void ShaderGraph::linkNodes(id_t sourceNodeId, id_t targetNodeId, std::uint32_t sourceSlot, std::uint32_t targetSlot) {
	if(!containsNode(sourceNodeId) || !containsNode(targetNodeId)) {
		return;
	}

	ShaderNode& sourceNode = shaderNodes[sourceNodeId];
	ShaderNode& targetNode = shaderNodes[targetNodeId];
	if(targetSlot >= targetNode.inputs().size() || sourceSlot >= nodeType(sourceNode.type()).outputs.size()) {
		return;
	}

	targetNode.inputs()[targetSlot] = ShaderNode::Link(
		nextLinkId++,
		sourceNodeId,
		sourceSlot);
}
void ShaderGraph::linkNodes(id_t sourceNodeId, id_t targetNodeId, const std::string& sourceSlotName, const std::string& targetSlotName) {
	if(!containsNode(sourceNodeId) || !containsNode(targetNodeId)) {
		return;
	}

	ShaderNode& sourceNode = shaderNodes[sourceNodeId];
	ShaderNode& targetNode = shaderNodes[targetNodeId];
	const ShaderNodeType& sourceNodeType = nodeType(sourceNode.type());
	const ShaderNodeType& targetNodeType = nodeType(targetNode.type());

	std::uint32_t sourceSlot = id_t::nullValue;
	for(std::size_t outputSlot = 0; outputSlot < sourceNodeType.outputs.size(); outputSlot++) {
		if(sourceNodeType.outputs[outputSlot] == sourceSlotName) {
			sourceSlot = static_cast<std::uint32_t>(outputSlot);
		}
	}

	std::uint32_t targetSlot = id_t::nullValue;
	for(std::size_t inputSlot = 0; inputSlot < targetNodeType.inputs.size(); inputSlot++) {
		if(targetNodeType.inputs[inputSlot] == targetSlotName) {
			targetSlot = static_cast<std::uint32_t>(inputSlot);
		}
	}

	if(sourceSlot != id_t::nullValue && targetSlot != id_t::nullValue) {
		targetNode.inputs()[targetSlot] = ShaderNode::Link(
			nextLinkId++,
			sourceNodeId,
			sourceSlot);
	}
}
void ShaderGraph::unlinkNodes(id_t sourceNodeId, id_t targetNodeId, std::uint32_t targetSlot) {
	if(!containsNode(sourceNodeId) || !containsNode(targetNodeId)) {
		return;
	}

	ShaderNode& targetNode = shaderNodes[targetNodeId];
	if(targetSlot >= targetNode.inputs().size()) {
		return;
	}

	targetNode.inputs()[targetSlot] = ShaderNode::Link();
}
void ShaderGraph::unlinkNodes(id_t linkId) {
	for(auto& nodeEntry : shaderNodes) {
		for(ShaderNode::Link& link : nodeEntry.second.inputs()) {
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

	shaderNodes[nodeId].name(name);
}
void ShaderGraph::nodeParameter(id_t nodeId, std::uint32_t parameterIndex, VariantParameter::Value value) {
	if(!containsNode(nodeId) || parameterIndex >= shaderNodes[nodeId].parameters().size()) {
		return;
	}

	shaderNodes[nodeId].parameters()[parameterIndex] = value;
}
void ShaderGraph::nodeParameter(id_t nodeId, const std::string& parameterName, VariantParameter::Value value) {
	if(!containsNode(nodeId)) {
		return;
	}

	ShaderNode& node = shaderNodes[nodeId];
	for(std::size_t parameterIndex = 0; parameterIndex < shaderNodes[nodeId].parameters().size(); parameterIndex++) {
		if(nodeType(node.type()).parameters[parameterIndex].name() == parameterName) {
			node.parameters()[parameterIndex] = value;
		}
	}
}
void ShaderGraph::nodeParameterNode(id_t nodeId, bool enable) {
	if(!containsNode(nodeId)) {
		return;
	}

	shaderNodes[nodeId].parameterNode(enable);
}
void ShaderGraph::nodePosition(id_t nodeId, const float2_t& position) {
	if(!containsNode(nodeId)) {
		return;
	}

	shaderNodes[nodeId].position(position);
}

bool ShaderGraph::containsNode(id_t nodeId) const {
	return shaderNodes.count(nodeId) != 0;
}
const ShaderNode& ShaderGraph::node(id_t nodeId) const {
	return shaderNodes.at(nodeId);
}
const ShaderGraph::ShaderNodeCollection& ShaderGraph::nodes() const {
	return shaderNodes;
}

std::unordered_map<id_t, VariantParameter> ShaderGraph::shaderParameters() const {
	std::unordered_map<id_t, VariantParameter> parameters;

	for(const auto& nodeEntry : shaderNodes) {
		const ShaderNode& node = nodeEntry.second;
		const ShaderNodeType& shaderNodeType = nodeType(node.type());

		if(node.parameterNode() && !shaderNodeType.parameters.empty()) {
			parameters[nodeEntry.first] = shaderNodeType.parameters[0];
		}
	}

	return parameters;
}

bool ShaderGraph::containsNodeType(const std::string& typeName) const{
	return findNodeType(typeName) != id_t::nullValue;
}
const ShaderNodeType& ShaderGraph::nodeType(const std::string& typeName) const {
	return graphLanguage.nodes.at(findNodeType(typeName));
}
const ShaderNodeType& ShaderGraph::nodeType(id_t nodeId) const {
	return graphLanguage.nodes.at(findNodeType(shaderNodes.at(nodeId).type()));
}

std::uint32_t ShaderGraph::findNodeType(const std::string& typeName) const {
	for(std::size_t nodeIndex = 0; nodeIndex < graphLanguage.nodes.size(); nodeIndex++) {
		if(graphLanguage.nodes[nodeIndex].name == typeName) {
			return static_cast<std::uint32_t>(nodeIndex);
		}
	}

	return id_t::nullValue;
}
std::uint32_t ShaderGraph::findNodeSignature(const BuildResult& result, const ShaderNode& node, std::vector<TypeMatch>& typeMatch) const {
	const auto matchTypes = [](VariantValue::Type type1, VariantValue::Type type2) {
		if(type1 == VariantValue::type_null || type2 == VariantValue::type_null) {
			return ShaderGraph::typematch_none;
		}
		else if(type1 == type2) {
			return ShaderGraph::typematch_exact;
		}
		else if(type1 > type2) {
			return ShaderGraph::typematch_downcast;
		}
		else if(type1 < type2 && (type1 == VariantValue::type_bool || type1 == VariantValue::type_int || type1 == VariantValue::type_float)) {
			return ShaderGraph::typematch_upcast;
		}

		return ShaderGraph::typematch_none;
	};

	const ShaderNodeType& shaderNodeType = nodeType(node.type());

	std::uint32_t bestSignature = id_t::nullValue;
	typeMatch = std::vector<TypeMatch>(shaderNodeType.inputs.size(), typematch_none);

	for(std::size_t signatureIndex = 0; signatureIndex < shaderNodeType.signatures.size(); signatureIndex++) {
		const ShaderNodeType::Signature& signature = shaderNodeType.signatures[signatureIndex];

		std::vector<TypeMatch> currentMatch(node.inputs().size(), typematch_none);
		for(std::size_t inputIndex = 0; inputIndex < node.inputs().size(); inputIndex++) {
			VariantValue::Type inputType = VariantValue::type_null;
			id_t sourceNodeId = node.inputs()[inputIndex].nodeId;

			if(shaderNodes.count(sourceNodeId) != 0) {
				std::uint32_t sourceSlot = node.inputs()[inputIndex].slot;
				std::uint32_t sourceSignature = result.nodeSignatures.at(sourceNodeId);
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
			if(currentMatch[inputIndex] == typematch_none || currentMatch[inputIndex] > typeMatch[inputIndex]) {
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
