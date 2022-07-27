#include "ShaderGraph.h"
#include "Curve.h"

namespace pixelpart {
namespace {
std::string replace(std::string str, const std::string& to, const std::string& from) {
	if(from.empty()) {
		return str;
	}

	std::size_t pos = 0;
	while((pos = str.find(from, pos)) != std::string::npos) {
		str.replace(pos, from.length(), to);
		pos += to.length();
	}

	return str;
}
}

ShaderGraph::BuildException::BuildException(const std::string& msg, uint32_t node, uint32_t slot) : std::runtime_error(msg), nodeId(node), slotIndex(slot) {

}

uint32_t ShaderGraph::BuildException::getNodeId() const {
	return nodeId;
}
uint32_t ShaderGraph::BuildException::getSlotIndex() const {
	return slotIndex;
}

ShaderGraphType ShaderGraph::graphType = ShaderGraphType();
std::string ShaderGraph::shaderVariableNamePrefix = "var";
uint32_t ShaderGraph::numCurveInterpolationPoints = 100;

ShaderGraph::ShaderGraph() {

}
ShaderGraph::ShaderGraph(const std::unordered_map<uint32_t, ShaderNode>& initialNodes) : nodes(initialNodes) {
	uint32_t maxNodeId = 0;
	uint32_t maxLinkId = 0;
	for(const auto& node : nodes) {
		maxNodeId = std::max(maxNodeId, node.first);

		for(const auto& link : node.second.inputs) {
			if(link.id == pixelpart::NullId) {
				continue;
			}

			maxLinkId = std::max(maxLinkId, link.id);
		}
	}

	nextNodeId = maxNodeId + 1;
	nextLinkId = maxLinkId + 1;
}

std::string ShaderGraph::build(BuildResult& result, uint32_t nodeId) const {
	if(nodeId == 0) {
		result = BuildResult();
	}

	auto nodeEntry = nodes.find(nodeId);
	if(nodeEntry == nodes.end()) {
		return std::string();
	}

	const ShaderNode& node = nodeEntry->second;

	uint32_t nodeTypeIndex = findNodeType(node.type);
	if(nodeTypeIndex == NullId) {
		throw BuildException("Unknown node type \"" + node.type + "\"", nodeId);
	}

	const ShaderNodeType& nodeType = graphType.nodes.at(nodeTypeIndex);

	std::string code = nodeType.code;

	for(uint32_t parameterIndex = 0; parameterIndex < static_cast<uint32_t>(node.parameters.size()); parameterIndex++) {
		const ShaderParameter::Value& parameterValue = node.parameters[parameterIndex];

		std::string valueString;
		switch(parameterValue.type) {
			case ShaderParameter::Value::type_int:
			case ShaderParameter::Value::type_enum: {
				valueString = graphType.typeConstructors[ShaderValue::type_int];
				valueString = replace(valueString, std::to_string(parameterValue.data.integer), "{0}");
				break;
			}
			case ShaderParameter::Value::type_float: {
				valueString = graphType.typeConstructors[ShaderValue::type_float];
				valueString = replace(valueString, std::to_string(parameterValue.data.numbers[0]), "{0}");
				break;
			}
			case ShaderParameter::Value::type_float2: {
				valueString = graphType.typeConstructors[ShaderValue::type_float2];
				valueString = replace(valueString, std::to_string(parameterValue.data.numbers[0]), "{0}");
				valueString = replace(valueString, std::to_string(parameterValue.data.numbers[1]), "{1}");
				break;
			}
			case ShaderParameter::Value::type_float3: {
				valueString = graphType.typeConstructors[ShaderValue::type_float3];
				valueString = replace(valueString, std::to_string(parameterValue.data.numbers[0]), "{0}");
				valueString = replace(valueString, std::to_string(parameterValue.data.numbers[1]), "{1}");
				valueString = replace(valueString, std::to_string(parameterValue.data.numbers[2]), "{2}");
				break;
			}
			case ShaderParameter::Value::type_float4:
			case ShaderParameter::Value::type_color: {
				valueString = graphType.typeConstructors[ShaderValue::type_float4];
				valueString = replace(valueString, std::to_string(parameterValue.data.numbers[0]), "{0}");
				valueString = replace(valueString, std::to_string(parameterValue.data.numbers[1]), "{1}");
				valueString = replace(valueString, std::to_string(parameterValue.data.numbers[2]), "{2}");
				valueString = replace(valueString, std::to_string(parameterValue.data.numbers[3]), "{3}");
				break;
			}
			case ShaderParameter::Value::type_bool: {
				valueString = graphType.typeConstructors[ShaderValue::type_bool];
				valueString = replace(valueString, (parameterValue.data.integer != 0) ? "true" : "false", "{0}");
				break;
			}
			case ShaderParameter::Value::type_curve: {
				Curve<floatd> curve = parameterValue.getCurve();

				for(uint32_t i = 0; i < numCurveInterpolationPoints; i++) {
					floatd pointValue = curve.get(static_cast<floatd>(i) / static_cast<floatd>(numCurveInterpolationPoints - 1));
					std::string pointValueString = graphType.typeConstructors[ShaderValue::type_float];
					pointValueString = replace(pointValueString, std::to_string(pointValue), "{0}");

					valueString += pointValueString;
					if(i + 1 < numCurveInterpolationPoints) {
						valueString += ", ";
					}
				}

				break;
			}
			case ShaderParameter::Value::type_path: {
				Curve<vec2d> path = parameterValue.getPath();

				for(uint32_t i = 0; i < numCurveInterpolationPoints; i++) {
					vec2d pointValue = path.get(static_cast<floatd>(i) / static_cast<floatd>(numCurveInterpolationPoints - 1));
					std::string pointValueString = graphType.typeConstructors[ShaderValue::type_float2];
					pointValueString = replace(pointValueString, std::to_string(pointValue.x), "{0}");
					pointValueString = replace(pointValueString, std::to_string(pointValue.y), "{1}");

					valueString += pointValueString;
					if(i + 1 < numCurveInterpolationPoints) {
						valueString += ", ";
					}
				}

				break;
			}
			case ShaderParameter::Value::type_gradient: {
				Curve<vec3d> gradient = parameterValue.getGradient();

				for(uint32_t i = 0; i < numCurveInterpolationPoints; i++) {
					vec3d pointValue = gradient.get(static_cast<floatd>(i) / static_cast<floatd>(numCurveInterpolationPoints - 1));
					std::string pointValueString = graphType.typeConstructors[ShaderValue::type_float3];
					pointValueString = replace(pointValueString, std::to_string(pointValue.x), "{0}");
					pointValueString = replace(pointValueString, std::to_string(pointValue.y), "{1}");
					pointValueString = replace(pointValueString, std::to_string(pointValue.z), "{2}");

					valueString += pointValueString;
					if(i + 1 < numCurveInterpolationPoints) {
						valueString += ", ";
					}
				}

				break;
			}
			case ShaderParameter::Value::type_resource_image: {
				std::size_t samplerIndex = result.textureIds.size();
				if(samplerIndex < graphType.textureSamplers.size()) {
					result.textureIds.push_back(parameterValue.getResourceId());
					valueString = graphType.textureSamplers[samplerIndex];
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

		code = replace(code, valueString, "{param" + std::to_string(parameterIndex) + "}");
	}

	std::string inputCode;
	for(uint32_t inputSlot = 0; inputSlot < static_cast<uint32_t>(node.inputs.size()); inputSlot++) {
		const ShaderNode::Link& link = node.inputs[inputSlot];

		if(nodes.count(link.nodeId) != 0) {
			if(result.resolvedNodes.count(link.nodeId) == 0) {
				inputCode += build(result, link.nodeId);
			}
		}
		else if(nodeType.defaultInputs[inputSlot].type != ShaderValue::type_null) {
			std::string valueString;
			switch(nodeType.defaultInputs[inputSlot].type) {
				case ShaderValue::type_bool: {
					valueString = graphType.typeConstructors[ShaderValue::type_bool];
					valueString = replace(valueString, nodeType.defaultInputs[inputSlot].data.boolean ? "true" : "false", "{0}");
					break;
				}
				case ShaderValue::type_int: {
					valueString = graphType.typeConstructors[ShaderValue::type_int];
					valueString = replace(valueString, std::to_string(nodeType.defaultInputs[inputSlot].data.integer), "{0}");
					break;
				}
				case ShaderValue::type_float: {
					valueString = graphType.typeConstructors[ShaderValue::type_float];
					valueString = replace(valueString, std::to_string(nodeType.defaultInputs[inputSlot].data.number[0]), "{0}");
					break;
				}
				case ShaderValue::type_float2: {
					valueString = graphType.typeConstructors[ShaderValue::type_float2];
					valueString = replace(valueString, std::to_string(nodeType.defaultInputs[inputSlot].data.number[0]), "{0}");
					valueString = replace(valueString, std::to_string(nodeType.defaultInputs[inputSlot].data.number[1]), "{1}");
					break;
				}
				case ShaderValue::type_float3: {
					valueString = graphType.typeConstructors[ShaderValue::type_float3];
					valueString = replace(valueString, std::to_string(nodeType.defaultInputs[inputSlot].data.number[0]), "{0}");
					valueString = replace(valueString, std::to_string(nodeType.defaultInputs[inputSlot].data.number[1]), "{1}");
					valueString = replace(valueString, std::to_string(nodeType.defaultInputs[inputSlot].data.number[2]), "{2}");
					break;
				}
				case ShaderValue::type_float4: {
					valueString = graphType.typeConstructors[ShaderValue::type_float4];
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
			throw BuildException("Input node not found", nodeId, inputSlot);
		}
	}

	std::vector<TypeMatch> typeMatch;
	result.nodeSignatures[nodeId] = findNodeSignature(result, node, typeMatch);
	if(result.nodeSignatures[nodeId] == NullId) {
		throw BuildException("No matching node signature found", nodeId);
	}

	const ShaderNodeType::Signature& signature = nodeType.signatures[result.nodeSignatures[nodeId]];

	result.nodeOutputVariables[nodeId] = std::vector<std::string>();
	for(uint32_t outputSlot = 0; outputSlot < static_cast<uint32_t>(nodeType.outputs.size()); outputSlot++) {
		std::string outputVariableName = shaderVariableNamePrefix + std::to_string(result.numVariables++);
		std::string outputVariableDefinition = graphType.typeNames[static_cast<uint32_t>(signature.outputTypes[outputSlot])] + " " + outputVariableName;
		result.nodeOutputVariables[nodeId].push_back(outputVariableName);

		code = replace(code, outputVariableName, "{out" + std::to_string(outputSlot) + "}");
		code = replace(code, outputVariableDefinition, "{outdef" + std::to_string(outputSlot) + "}");
		result.numVariables++;
	}

	for(uint32_t inputSlot = 0; inputSlot < static_cast<uint32_t>(node.inputs.size()); inputSlot++) {
		const ShaderNode::Link& link = node.inputs[inputSlot];
		if(typeMatch[inputSlot] == typematch_none) {
			throw BuildException("Types do not match", nodeId, inputSlot);
		}

		ShaderValue::Type sourceValueType = ShaderValue::type_null;

		if(nodes.count(link.nodeId) != 0) {
			const ShaderNodeType& sourceNodeType = getNodeTypeOfNode(link.nodeId);
			const ShaderNodeType::Signature& sourceNodeSignature = sourceNodeType.signatures[result.nodeSignatures[link.nodeId]];

			sourceValueType = sourceNodeSignature.outputTypes[link.slot];
		}
		else {
			sourceValueType = nodeType.defaultInputs[inputSlot].type;
		}
		
		std::string inputVariableTemplate = "{in" + std::to_string(inputSlot) + "}";

		if(typeMatch[inputSlot] != typematch_exact) {
			std::string codeTypeCast = graphType.typeCasts[signature.inputTypes[inputSlot]][sourceValueType];
			codeTypeCast = replace(codeTypeCast, inputVariableTemplate, "{0}");
			code = replace(code, codeTypeCast, inputVariableTemplate);
		}

		code = replace(code, result.nodeOutputVariables[link.nodeId][link.slot], inputVariableTemplate);
	}

	result.resolvedNodes.insert(nodeId);

	if(nodeId == 0) {
		return result.code = replace(graphType.shaderTemplate, inputCode + "\n" + code, "{0}");
	}

	return inputCode + "\n" + code;
}

uint32_t ShaderGraph::addNode(const std::string& typeName) {
	uint32_t typeIndex = findNodeType(typeName);
	if(typeIndex == NullId) {
		return NullId;
	}

	uint32_t nodeId = nextNodeId++;
	nodes[nodeId] = ShaderNode(graphType.nodes[typeIndex]);

	return nodeId;
}
void ShaderGraph::removeNode(uint32_t nodeId) {
	for(auto& nodeEntry : nodes) {
		for(auto& link : nodeEntry.second.inputs) {
			if(link.nodeId == nodeId) {
				link = ShaderNode::Link();
			}
		}
	}

	nodes.erase(nodeId);
}
void ShaderGraph::linkNodes(uint32_t sourceNodeId, uint32_t targetNodeId, uint32_t sourceSlot, uint32_t targetSlot) {
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
void ShaderGraph::linkNodes(uint32_t sourceNodeId, uint32_t targetNodeId, const std::string& sourceSlotName, const std::string& targetSlotName) {
	if(!hasNode(sourceNodeId) || !hasNode(targetNodeId)) {
		return;
	}

	ShaderNode& sourceNode = nodes[sourceNodeId];
	ShaderNode& targetNode = nodes[targetNodeId];
	const ShaderNodeType& sourceNodeType = getNodeType(sourceNode.type);
	const ShaderNodeType& targetNodeType = getNodeType(targetNode.type);

	uint32_t sourceSlot = NullId;
	for(uint32_t o = 0; o < sourceNodeType.outputs.size(); o++) {
		if(sourceNodeType.outputs[o] == sourceSlotName) {
			sourceSlot = o;
		}
	}

	uint32_t targetSlot = NullId;
	for(uint32_t i = 0; i < targetNodeType.inputs.size(); i++) {
		if(targetNodeType.inputs[i] == targetSlotName) {
			targetSlot = i;
		}
	}

	if(sourceSlot != NullId && targetSlot != NullId) {
		targetNode.inputs[targetSlot] = ShaderNode::Link(
			nextLinkId++,
			sourceNodeId,
			sourceSlot);
	}
}
void ShaderGraph::unlinkNodes(uint32_t sourceNodeId, uint32_t targetNodeId, uint32_t targetSlot) {
	if(!hasNode(sourceNodeId) || !hasNode(targetNodeId)) {
		return;
	}

	ShaderNode& targetNode = nodes[targetNodeId];
	if(targetSlot >= targetNode.inputs.size()) {
		return;
	}

	targetNode.inputs[targetSlot] = ShaderNode::Link();
}
void ShaderGraph::unlinkNodes(uint32_t linkId) {
	for(auto& nodeEntry : nodes) {
		for(ShaderNode::Link& link : nodeEntry.second.inputs) {
			if(link.id == linkId) {
				link = ShaderNode::Link();
				return;
			}
		}
	}
}
void ShaderGraph::setNodeParameter(uint32_t nodeId, uint32_t parameterIndex, ShaderParameter::Value value) {
	if(!hasNode(nodeId) || parameterIndex >= nodes[nodeId].parameters.size()) {
		return;
	}

	nodes[nodeId].parameters[parameterIndex] = value;
}
void ShaderGraph::setNodeParameter(uint32_t nodeId, const std::string& parameterName, ShaderParameter::Value value) {
	if(!hasNode(nodeId)) {
		return;
	}

	ShaderNode& node = nodes[nodeId];
	for(uint32_t p = 0; p < nodes[nodeId].parameters.size(); p++) {
		if(getNodeType(node.type).parameters[p].name == parameterName) {
			node.parameters[p] = value;
		}
	}
}
void ShaderGraph::setNodePosition(uint32_t nodeId, const vec2d& position) {
	if(!hasNode(nodeId)) {
		return;
	}

	nodes[nodeId].position = position;
}
bool ShaderGraph::hasNode(uint32_t nodeId) const {
	return nodes.count(nodeId) != 0;
}
const ShaderNode& ShaderGraph::getNode(uint32_t nodeId) const {
	return nodes.at(nodeId);
}
const std::unordered_map<uint32_t, ShaderNode>& ShaderGraph::getNodes() const {
	return nodes;
}
uint32_t ShaderGraph::getNextNodeId() const {
	return nextNodeId;
}
uint32_t ShaderGraph::getNextLinkId() const {
	return nextLinkId;
}

bool ShaderGraph::hasNodeType(const std::string& typeName) const{
	return findNodeType(typeName) != NullId;
}
const ShaderNodeType& ShaderGraph::getNodeType(const std::string& typeName) const {
	return graphType.nodes.at(findNodeType(typeName));
}
const ShaderNodeType& ShaderGraph::getNodeTypeOfNode(uint32_t nodeId) const {
	return graphType.nodes.at(findNodeType(nodes.at(nodeId).type));
}

uint32_t ShaderGraph::findNodeType(const std::string& typeName) const {
	uint32_t typeIndex = NullId;
	for(uint32_t i = 0; i < graphType.nodes.size(); i++) {
		if(graphType.nodes[i].name == typeName) {
			typeIndex = i;
			break;
		}
	}

	return typeIndex;
}
uint32_t ShaderGraph::findNodeSignature(const BuildResult& result, const ShaderNode& node, std::vector<TypeMatch>& typeMatch) const {
	const auto matchTypes = [](ShaderValue::Type type1, ShaderValue::Type type2) {
		if(type1 == ShaderValue::type_null || type2 == ShaderValue::type_null) {
			return ShaderGraph::typematch_none;
		}
		else if(type1 == type2) {
			return ShaderGraph::typematch_exact;
		}
		else if(type1 > type2) {
			return ShaderGraph::typematch_downcast;
		}
		else if(type1 < type2 && (type1 == ShaderValue::type_bool || type1 == ShaderValue::type_int || type1 == ShaderValue::type_float)) {
			return ShaderGraph::typematch_upcast;
		}

		return ShaderGraph::typematch_none;
	};

	const ShaderNodeType& nodeType = getNodeType(node.type);

	uint32_t bestSignature = NullId;
	typeMatch = std::vector<TypeMatch>(nodeType.inputs.size(), typematch_none);

	for(uint32_t s = 0; s < static_cast<uint32_t>(nodeType.signatures.size()); s++) {
		const ShaderNodeType::Signature& signature = nodeType.signatures[s];
		
		std::vector<TypeMatch> currentMatch(node.inputs.size(), typematch_none);
		for(uint32_t i = 0; i < static_cast<uint32_t>(node.inputs.size()); i++) {
			ShaderValue::Type inputType = ShaderValue::type_null;
			uint32_t sourceNodeId = node.inputs[i].nodeId;

			if(nodes.count(sourceNodeId) != 0) {
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
		for(uint32_t i = 0; i < static_cast<uint32_t>(node.inputs.size()); i++) {
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
			bestSignature = s;
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
		j.at("nodes").get<std::unordered_map<uint32_t, ShaderNode>>()
	);
}
}