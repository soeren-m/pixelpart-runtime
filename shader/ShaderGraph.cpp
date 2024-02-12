#include "ShaderGraph.h"

namespace pixelpart {
namespace {
std::string replace(std::string str, const std::string& to, const std::string& from) {
	if(from.empty()) {
		return str;
	}

	std::size_t pos = 0u;
	while((pos = str.find(from, pos)) != std::string::npos) {
		str.replace(pos, from.length(), to);
		pos += to.length();
	}

	return str;
}
}

ShaderGraph::BuildException::BuildException(const std::string& msg, id_t node, uint32_t slot) : std::runtime_error(msg), nodeId(node), slotIndex(slot) {

}

id_t ShaderGraph::BuildException::getNodeId() const {
	return nodeId;
}
uint32_t ShaderGraph::BuildException::getSlotIndex() const {
	return slotIndex;
}

ShaderGraphLanguage ShaderGraph::graphLanguage = ShaderGraphLanguage();

uint32_t ShaderGraph::numCurveInterpolationPoints = 100u;

ShaderGraph::ShaderGraph() {

}
ShaderGraph::ShaderGraph(const std::unordered_map<id_t, ShaderNode>& initialNodes) : nodes(initialNodes) {
	id_t maxNodeId = 0u;
	id_t maxLinkId = 0u;
	for(const auto& nodeEntry : nodes) {
		maxNodeId = std::max(maxNodeId, nodeEntry.first);

		for(const auto& link : nodeEntry.second.inputs) {
			if(link.id == pixelpart::nullId) {
				continue;
			}

			maxLinkId = std::max(maxLinkId, link.id);
		}
	}

	nextNodeId = maxNodeId + 1u;
	nextLinkId = maxLinkId + 1u;
}

std::string ShaderGraph::build(BuildResult& result, id_t nodeId) const {
	if(nodeId == 0u) {
		result = BuildResult();
	}

	auto nodeEntry = nodes.find(nodeId);
	if(nodeEntry == nodes.end()) {
		return std::string();
	}

	const ShaderNode& node = nodeEntry->second;

	uint32_t nodeTypeIndex = findNodeType(node.type);
	if(nodeTypeIndex == nullId) {
		throw BuildException("Unknown node type \"" + node.type + "\"", nodeId);
	}

	const ShaderNodeType& nodeType = graphLanguage.nodes.at(nodeTypeIndex);

	std::string code = nodeType.code;

	for(std::size_t parameterIndex = 0u; parameterIndex < node.parameters.size(); parameterIndex++) {
		const VariantParameter& parameter = nodeType.parameters.at(parameterIndex);
		const VariantParameter::Value& parameterValue = node.parameters.at(parameterIndex);
		std::string valueString;

		if(node.isParameterNode) {
			if(parameterValue.type == VariantParameter::Value::type_resource_image) {
				std::size_t samplerIndex = result.textureResourceIds.size();
				if(samplerIndex < graphLanguage.textureSamplers.size()) {
					result.textureResourceIds.push_back(parameterValue.getResourceId());
					result.textureParameterNames.push_back(parameter.name);
					result.parameterNames[nodeId] = graphLanguage.textureSamplers[samplerIndex];

					valueString = graphLanguage.textureSamplers[samplerIndex];
				}
				else {
					throw BuildException("No more texture samplers available", nodeId);
				}
			}
			else {
				std::string nodeIdentifier = replace(nodeType.name, "_", " ");
				std::string parameterVariableName = graphLanguage.parameterPrefix
					+ nodeIdentifier
					+ "_" + std::to_string(nodeId)
					+ "_" + std::to_string(parameterIndex);

				result.parameterNames[nodeId] = parameterVariableName;

				std::string parameterCode = graphLanguage.parameterTemplate;
				parameterCode = replace(parameterCode, graphLanguage.parameterTypeNames.at(parameterValue.type), "{type}");
				parameterCode = replace(parameterCode, parameterVariableName, "{name}");
				result.parameterCode += parameterCode;
				result.parameterCode += "\n";

				valueString = parameterVariableName;
			}
		}
		else {
			switch(parameterValue.type) {
				case VariantParameter::Value::type_int:
				case VariantParameter::Value::type_enum: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_int];
					valueString = replace(valueString, std::to_string(parameterValue.data.integer), "{0}");
					break;
				}
				case VariantParameter::Value::type_float: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_float];
					valueString = replace(valueString, std::to_string(parameterValue.data.numbers[0]), "{0}");
					break;
				}
				case VariantParameter::Value::type_float2: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_float2];
					valueString = replace(valueString, std::to_string(parameterValue.data.numbers[0]), "{0}");
					valueString = replace(valueString, std::to_string(parameterValue.data.numbers[1]), "{1}");
					break;
				}
				case VariantParameter::Value::type_float3: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_float3];
					valueString = replace(valueString, std::to_string(parameterValue.data.numbers[0]), "{0}");
					valueString = replace(valueString, std::to_string(parameterValue.data.numbers[1]), "{1}");
					valueString = replace(valueString, std::to_string(parameterValue.data.numbers[2]), "{2}");
					break;
				}
				case VariantParameter::Value::type_float4:
				case VariantParameter::Value::type_color: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_float4];
					valueString = replace(valueString, std::to_string(parameterValue.data.numbers[0]), "{0}");
					valueString = replace(valueString, std::to_string(parameterValue.data.numbers[1]), "{1}");
					valueString = replace(valueString, std::to_string(parameterValue.data.numbers[2]), "{2}");
					valueString = replace(valueString, std::to_string(parameterValue.data.numbers[3]), "{3}");
					break;
				}
				case VariantParameter::Value::type_bool: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_bool];
					valueString = replace(valueString, (parameterValue.data.integer != 0) ? "true" : "false", "{0}");
					break;
				}
				case VariantParameter::Value::type_curve: {
					Curve<float_t> curve = parameterValue.getCurve();

					for(uint32_t i = 0u; i < numCurveInterpolationPoints; i++) {
						float_t pointValue = curve.get(static_cast<float_t>(i) / static_cast<float_t>(numCurveInterpolationPoints - 1u));
						std::string pointValueString = graphLanguage.typeConstructors[VariantValue::type_float];
						pointValueString = replace(pointValueString, std::to_string(pointValue), "{0}");

						valueString += pointValueString;
						if(i + 1u < numCurveInterpolationPoints) {
							valueString += ", ";
						}
					}

					break;
				}
				case VariantParameter::Value::type_gradient: {
					Curve<vec3_t> gradient = parameterValue.getGradient();

					for(uint32_t i = 0u; i < numCurveInterpolationPoints; i++) {
						vec3_t pointValue = gradient.get(static_cast<float_t>(i) / static_cast<float_t>(numCurveInterpolationPoints - 1u));
						std::string pointValueString = graphLanguage.typeConstructors[VariantValue::type_float3];
						pointValueString = replace(pointValueString, std::to_string(pointValue.x), "{0}");
						pointValueString = replace(pointValueString, std::to_string(pointValue.y), "{1}");
						pointValueString = replace(pointValueString, std::to_string(pointValue.z), "{2}");

						valueString += pointValueString;
						if(i + 1u < numCurveInterpolationPoints) {
							valueString += ", ";
						}
					}

					break;
				}
				case VariantParameter::Value::type_resource_image: {
					std::size_t samplerIndex = result.textureResourceIds.size();
					if(samplerIndex < graphLanguage.textureSamplers.size()) {
						result.textureResourceIds.push_back(parameterValue.getResourceId());
						result.textureParameterNames.push_back(parameter.name);
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

		code = replace(code, valueString, "{param" + std::to_string(parameterIndex) + "}");
	}

	std::string inputCode;
	for(std::size_t inputSlot = 0u; inputSlot < node.inputs.size(); inputSlot++) {
		const ShaderNode::Link& link = node.inputs[inputSlot];

		if(nodes.count(link.nodeId) != 0u) {
			if(result.resolvedNodes.count(link.nodeId) == 0u) {
				inputCode += build(result, link.nodeId);
			}
		}
		else if(nodeType.defaultInputs[inputSlot].type != VariantValue::type_null) {
			std::string valueString;
			switch(nodeType.defaultInputs[inputSlot].type) {
				case VariantValue::type_bool: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_bool];
					valueString = replace(valueString, nodeType.defaultInputs[inputSlot].data.boolean ? "true" : "false", "{0}");
					break;
				}
				case VariantValue::type_int: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_int];
					valueString = replace(valueString, std::to_string(nodeType.defaultInputs[inputSlot].data.integer), "{0}");
					break;
				}
				case VariantValue::type_float: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_float];
					valueString = replace(valueString, std::to_string(nodeType.defaultInputs[inputSlot].data.number[0]), "{0}");
					break;
				}
				case VariantValue::type_float2: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_float2];
					valueString = replace(valueString, std::to_string(nodeType.defaultInputs[inputSlot].data.number[0]), "{0}");
					valueString = replace(valueString, std::to_string(nodeType.defaultInputs[inputSlot].data.number[1]), "{1}");
					break;
				}
				case VariantValue::type_float3: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_float3];
					valueString = replace(valueString, std::to_string(nodeType.defaultInputs[inputSlot].data.number[0]), "{0}");
					valueString = replace(valueString, std::to_string(nodeType.defaultInputs[inputSlot].data.number[1]), "{1}");
					valueString = replace(valueString, std::to_string(nodeType.defaultInputs[inputSlot].data.number[2]), "{2}");
					break;
				}
				case VariantValue::type_float4: {
					valueString = graphLanguage.typeConstructors[VariantValue::type_float4];
					valueString = replace(valueString, std::to_string(nodeType.defaultInputs[inputSlot].data.number[0]), "{0}");
					valueString = replace(valueString, std::to_string(nodeType.defaultInputs[inputSlot].data.number[1]), "{1}");
					valueString = replace(valueString, std::to_string(nodeType.defaultInputs[inputSlot].data.number[2]), "{2}");
					valueString = replace(valueString, std::to_string(nodeType.defaultInputs[inputSlot].data.number[3]), "{3}");
					break;
				}
				default: {
					break;
				}
			}

			code = replace(code, valueString, "{in" + std::to_string(inputSlot) + "}");
		}
		else {
			throw BuildException("Input node not found", nodeId, static_cast<uint32_t>(inputSlot));
		}
	}

	std::vector<TypeMatch> typeMatch;
	result.nodeSignatures[nodeId] = findNodeSignature(result, node, typeMatch);
	if(result.nodeSignatures[nodeId] == nullId) {
		throw BuildException("No matching node signature found", nodeId);
	}

	const ShaderNodeType::Signature& signature = nodeType.signatures[result.nodeSignatures[nodeId]];

	result.nodeOutputVariables[nodeId] = std::vector<std::string>();
	for(std::size_t outputSlot = 0u; outputSlot < nodeType.outputs.size(); outputSlot++) {
		std::string outputVariableName = graphLanguage.variablePrefix + std::to_string(result.numVariables++);
		std::string outputVariableDefinition = graphLanguage.typeNames[static_cast<std::size_t>(signature.outputTypes[outputSlot])] + " " + outputVariableName;
		result.nodeOutputVariables[nodeId].push_back(outputVariableName);

		code = replace(code, outputVariableName, "{out" + std::to_string(outputSlot) + "}");
		code = replace(code, outputVariableDefinition, "{outdef" + std::to_string(outputSlot) + "}");
		result.numVariables++;
	}

	for(std::size_t inputSlot = 0u; inputSlot < node.inputs.size(); inputSlot++) {
		const ShaderNode::Link& link = node.inputs[inputSlot];
		if(typeMatch[inputSlot] == typematch_none) {
			throw BuildException("Types do not match", nodeId, inputSlot);
		}

		VariantValue::Type sourceValueType = VariantValue::type_null;

		if(nodes.count(link.nodeId) != 0u) {
			const ShaderNodeType& sourceNodeType = getNodeTypeOfNode(link.nodeId);
			const ShaderNodeType::Signature& sourceNodeSignature = sourceNodeType.signatures[result.nodeSignatures[link.nodeId]];

			sourceValueType = sourceNodeSignature.outputTypes[link.slot];
		}
		else {
			sourceValueType = nodeType.defaultInputs[inputSlot].type;
		}

		std::string inputVariableTemplate = "{in" + std::to_string(inputSlot) + "}";

		if(typeMatch[inputSlot] != typematch_exact) {
			std::string codeTypeCast = graphLanguage.typeCasts[signature.inputTypes[inputSlot]][sourceValueType];
			codeTypeCast = replace(codeTypeCast, inputVariableTemplate, "{0}");
			code = replace(code, codeTypeCast, inputVariableTemplate);
		}

		code = replace(code, result.nodeOutputVariables[link.nodeId][link.slot], inputVariableTemplate);
	}

	result.resolvedNodes.insert(nodeId);

	if(nodeId == 0u) {
		std::string parameterBlock;
		if(!result.parameterCode.empty()) {
			parameterBlock = graphLanguage.parameterBlockTemplate;
			parameterBlock = replace(parameterBlock, graphLanguage.parameterBlockName, "{block_name}");
			parameterBlock = replace(parameterBlock, result.parameterCode, "{parameters}");
		}

		result.code = replace(graphLanguage.shaderTemplate, parameterBlock, "{parameter_block}");
		result.code = replace(result.code, inputCode + "\n" + code, "{main}");

		return result.code;
	}

	return inputCode + "\n" + code;
}

id_t ShaderGraph::addNode(const std::string& typeName) {
	uint32_t typeIndex = findNodeType(typeName);
	if(typeIndex == nullId) {
		return nullId;
	}

	id_t nodeId = nextNodeId++;
	nodes[nodeId] = ShaderNode(graphLanguage.nodes[typeIndex]);

	return nodeId;
}
void ShaderGraph::removeNode(id_t nodeId) {
	for(auto& nodeEntry : nodes) {
		for(auto& link : nodeEntry.second.inputs) {
			if(link.nodeId == nodeId) {
				link = ShaderNode::Link();
			}
		}
	}

	nodes.erase(nodeId);
}
void ShaderGraph::linkNodes(id_t sourceNodeId, id_t targetNodeId, uint32_t sourceSlot, uint32_t targetSlot) {
	if(!hasNode(sourceNodeId) || !hasNode(targetNodeId)) {
		return;
	}

	ShaderNode& sourceNode = nodes[sourceNodeId];
	ShaderNode& targetNode = nodes[targetNodeId];
	if(targetSlot >= targetNode.inputs.size() || sourceSlot >= getNodeType(sourceNode.type).outputs.size()) {
		return;
	}

	targetNode.inputs[targetSlot] = ShaderNode::Link(
		nextLinkId++,
		sourceNodeId,
		sourceSlot);
}
void ShaderGraph::linkNodes(id_t sourceNodeId, id_t targetNodeId, const std::string& sourceSlotName, const std::string& targetSlotName) {
	if(!hasNode(sourceNodeId) || !hasNode(targetNodeId)) {
		return;
	}

	ShaderNode& sourceNode = nodes[sourceNodeId];
	ShaderNode& targetNode = nodes[targetNodeId];
	const ShaderNodeType& sourceNodeType = getNodeType(sourceNode.type);
	const ShaderNodeType& targetNodeType = getNodeType(targetNode.type);

	uint32_t sourceSlot = nullId;
	for(std::size_t o = 0u; o < sourceNodeType.outputs.size(); o++) {
		if(sourceNodeType.outputs[o] == sourceSlotName) {
			sourceSlot = static_cast<uint32_t>(o);
		}
	}

	uint32_t targetSlot = nullId;
	for(std::size_t i = 0u; i < targetNodeType.inputs.size(); i++) {
		if(targetNodeType.inputs[i] == targetSlotName) {
			targetSlot = static_cast<uint32_t>(i);
		}
	}

	if(sourceSlot != nullId && targetSlot != nullId) {
		targetNode.inputs[targetSlot] = ShaderNode::Link(
			nextLinkId++,
			sourceNodeId,
			sourceSlot);
	}
}
void ShaderGraph::unlinkNodes(id_t sourceNodeId, id_t targetNodeId, uint32_t targetSlot) {
	if(!hasNode(sourceNodeId) || !hasNode(targetNodeId)) {
		return;
	}

	ShaderNode& targetNode = nodes[targetNodeId];
	if(targetSlot >= targetNode.inputs.size()) {
		return;
	}

	targetNode.inputs[targetSlot] = ShaderNode::Link();
}
void ShaderGraph::unlinkNodes(id_t linkId) {
	for(auto& nodeEntry : nodes) {
		for(ShaderNode::Link& link : nodeEntry.second.inputs) {
			if(link.id == linkId) {
				link = ShaderNode::Link();
				return;
			}
		}
	}
}
void ShaderGraph::setNodeName(id_t nodeId, const std::string& name) {
	if(!hasNode(nodeId)) {
		return;
	}

	nodes[nodeId].name = name;
}
void ShaderGraph::setNodeParameter(id_t nodeId, uint32_t parameterIndex, VariantParameter::Value value) {
	if(!hasNode(nodeId) || parameterIndex >= nodes[nodeId].parameters.size()) {
		return;
	}

	nodes[nodeId].parameters[parameterIndex] = value;
}
void ShaderGraph::setNodeParameter(id_t nodeId, const std::string& parameterName, VariantParameter::Value value) {
	if(!hasNode(nodeId)) {
		return;
	}

	ShaderNode& node = nodes[nodeId];
	for(std::size_t p = 0u; p < nodes[nodeId].parameters.size(); p++) {
		if(getNodeType(node.type).parameters[p].name == parameterName) {
			node.parameters[p] = value;
		}
	}
}
void ShaderGraph::setNodeParameterNode(id_t nodeId, bool enable) {
	if(!hasNode(nodeId)) {
		return;
	}

	nodes[nodeId].isParameterNode = enable;
}
void ShaderGraph::setNodePosition(id_t nodeId, const vec2_t& position) {
	if(!hasNode(nodeId)) {
		return;
	}

	nodes[nodeId].position = position;
}
bool ShaderGraph::hasNode(id_t nodeId) const {
	return nodes.count(nodeId) != 0u;
}
const ShaderNode& ShaderGraph::getNode(id_t nodeId) const {
	return nodes.at(nodeId);
}
const std::unordered_map<id_t, ShaderNode>& ShaderGraph::getNodes() const {
	return nodes;
}
id_t ShaderGraph::getNextNodeId() const {
	return nextNodeId;
}
id_t ShaderGraph::getNextLinkId() const {
	return nextLinkId;
}

std::unordered_map<id_t, VariantParameter> ShaderGraph::getShaderParameters() const {
	std::unordered_map<id_t, VariantParameter> parameters;

	for(const auto& nodeEntry : nodes) {
		const ShaderNode& node = nodeEntry.second;
		const ShaderNodeType& nodeType = getNodeType(node.type);

		if(node.isParameterNode && !nodeType.parameters.empty()) {
			parameters[nodeEntry.first] = nodeType.parameters[0];
		}
	}

	return parameters;
}

bool ShaderGraph::hasNodeType(const std::string& typeName) const{
	return findNodeType(typeName) != nullId;
}
const ShaderNodeType& ShaderGraph::getNodeType(const std::string& typeName) const {
	return graphLanguage.nodes.at(findNodeType(typeName));
}
const ShaderNodeType& ShaderGraph::getNodeTypeOfNode(uint32_t nodeId) const {
	return graphLanguage.nodes.at(findNodeType(nodes.at(nodeId).type));
}

uint32_t ShaderGraph::findNodeType(const std::string& typeName) const {
	for(std::size_t i = 0u; i < graphLanguage.nodes.size(); i++) {
		if(graphLanguage.nodes[i].name == typeName) {
			return static_cast<uint32_t>(i);
		}
	}

	return nullId;
}
uint32_t ShaderGraph::findNodeSignature(const BuildResult& result, const ShaderNode& node, std::vector<TypeMatch>& typeMatch) const {
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

	const ShaderNodeType& nodeType = getNodeType(node.type);

	uint32_t bestSignature = nullId;
	typeMatch = std::vector<TypeMatch>(nodeType.inputs.size(), typematch_none);

	for(std::size_t s = 0u; s < nodeType.signatures.size(); s++) {
		const ShaderNodeType::Signature& signature = nodeType.signatures[s];

		std::vector<TypeMatch> currentMatch(node.inputs.size(), typematch_none);
		for(std::size_t i = 0u; i < node.inputs.size(); i++) {
			VariantValue::Type inputType = VariantValue::type_null;
			id_t sourceNodeId = node.inputs[i].nodeId;

			if(nodes.count(sourceNodeId) != 0u) {
				uint32_t sourceSlot = node.inputs[i].slot;
				uint32_t sourceSignature = result.nodeSignatures.at(sourceNodeId);
				const ShaderNodeType& sourceNodeType = getNodeTypeOfNode(sourceNodeId);

				inputType = sourceNodeType.signatures[sourceSignature].outputTypes[sourceSlot];
			}
			else {
				inputType = nodeType.defaultInputs[i].type;
			}

			currentMatch[i] = matchTypes(inputType, signature.inputTypes[i]);
		}

		bool matchNotWorse = true;
		bool matchBetterForAnyInput = node.inputs.empty();
		for(std::size_t i = 0u; i < node.inputs.size(); i++) {
			if(currentMatch[i] == typematch_none || currentMatch[i] > typeMatch[i]) {
				matchNotWorse = false;
				break;
			}
			else if(currentMatch[i] < typeMatch[i]) {
				matchBetterForAnyInput = true;
			}
		}

		if(matchNotWorse && matchBetterForAnyInput) {
			typeMatch = currentMatch;
			bestSignature = static_cast<uint32_t>(s);
		}
	}

	return bestSignature;
}

void to_json(nlohmann::ordered_json& j, const ShaderGraph& shader) {
	j = nlohmann::ordered_json{
		{ "nodes", shader.getNodes() }
	};
}
void from_json(const nlohmann::ordered_json& j, ShaderGraph& shader) {
	shader = ShaderGraph(
		j.at("nodes").get<std::unordered_map<id_t, ShaderNode>>()
	);
}
}