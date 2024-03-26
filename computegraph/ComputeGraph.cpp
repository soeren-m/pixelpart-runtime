#include "ComputeGraph.h"
#include <algorithm>

namespace pixelpart {
ComputeGraph::EvaluationException::EvaluationException(const std::string& msg, id_t node, uint32_t slot) :
	std::runtime_error(msg), nodeId(node), slotIndex(slot) {

}

id_t ComputeGraph::EvaluationException::getNodeId() const {
	return nodeId;
}
uint32_t ComputeGraph::EvaluationException::getSlotIndex() const {
	return slotIndex;
}

ComputeNodeFactory ComputeGraph::nodeFactory = ComputeNodeFactory();

ComputeGraph::ComputeGraph() {

}
ComputeGraph::ComputeGraph(const ComputeGraph& other) {
	for(const auto& node : other.nodes) {
		if(node.second) {
			nodes[node.first] = std::move(node.second->clone());
		}
	}

	nextNodeId = other.nextNodeId;
	nextLinkId = other.nextLinkId;
}
ComputeGraph::ComputeGraph(const std::unordered_map<id_t, std::unique_ptr<ComputeNode>>& initialNodes) {
	for(const auto& node : initialNodes) {
		if(node.second) {
			nodes[node.first] = std::move(node.second->clone());
		}
	}

	id_t maxNodeId = 0u;
	id_t maxLinkId = 0u;

	for(const auto& node : nodes) {
		maxNodeId = std::max(maxNodeId, node.first);

		for(const auto& link : node.second->inputs) {
			if(link.id == pixelpart::nullId) {
				continue;
			}

			maxLinkId = std::max(maxLinkId, link.id);
		}
	}

	nextNodeId = maxNodeId + 1u;
	nextLinkId = maxLinkId + 1u;
}

ComputeGraph& ComputeGraph::operator=(const ComputeGraph& other) {
	nodes.clear();
	for(const auto& node : other.nodes) {
		if(node.second) {
			nodes[node.first] = std::move(node.second->clone());
		}
	}

	nextNodeId = other.nextNodeId;
	nextLinkId = other.nextLinkId;

	return *this;
}

std::vector<VariantValue> ComputeGraph::evaluate(const InputSet& graphInputs) const {
	BuildResult buildResult;

	return evaluate(graphInputs, buildResult);
}
std::vector<VariantValue> ComputeGraph::evaluate(const InputSet& graphInputs, BuildResult& result, id_t nodeId) const {
	const auto& nodeEntry = nodes.find(nodeId);
	if(nodeEntry == nodes.end()) {
		return std::vector<VariantValue>();
	}

	const std::unique_ptr<ComputeNode>& node = nodeEntry->second;
	std::vector<VariantValue> in(node->inputs.size());

	for(uint32_t inputSlot = 0u; inputSlot < node->inputs.size(); inputSlot++) {
		const ComputeNode::Link& link = node->inputs[inputSlot];

		VariantValue::Type sourceValueType = VariantValue::type_null;
		VariantValue inputValue;

		if(hasNode(link.nodeId)) {
			if(result.nodeOutputs.count(link.nodeId) == 0u) {
				result.nodeOutputs[link.nodeId] = evaluate(graphInputs, result, link.nodeId);
			}

			if(link.slot >= result.nodeOutputs[link.nodeId].size()) {
				throw EvaluationException("Node links to non-existent slot", nodeId, inputSlot);
			}

			const ComputeNode& sourceNode = getNode(link.nodeId);
			const ComputeNode::Signature& sourceNodeSignature = sourceNode.signatures[result.nodeSignatures[link.nodeId]];

			sourceValueType = sourceNodeSignature.outputTypes[link.slot];
			inputValue = result.nodeOutputs[link.nodeId][link.slot];
		}
		else if(link.nodeId == pixelpart::nullId && graphInputs.count(link.slot) != 0u) {
			sourceValueType = graphInputs.at(link.slot).type;
			inputValue = graphInputs.at(link.slot);
		}
		else if(inputSlot < node->defaultInputs.size() && node->defaultInputs[inputSlot].type != VariantValue::type_null) {
			sourceValueType = node->defaultInputs[inputSlot].type;
			inputValue = node->defaultInputs[inputSlot];
		}
		else {
			throw EvaluationException("Input node not found", nodeId, inputSlot);
		}

		in[inputSlot] = inputValue.cast(sourceValueType);
	}

	std::vector<TypeMatch> typeMatch;
	result.nodeSignatures[nodeId] = findNodeSignature(graphInputs, result, *node, typeMatch);
	if(result.nodeSignatures[nodeId] == nullId) {
		throw EvaluationException("No matching node signature found", nodeId);
	}

	for(uint32_t inputSlot = 0u; inputSlot < node->inputs.size(); inputSlot++) {
		if(typeMatch[inputSlot] == typematch_none) {
			throw EvaluationException("Types do not match", nodeId, inputSlot);
		}
	}

	return node->evaluate(in);
}

id_t ComputeGraph::addNode(const std::string& typeName) {
	id_t nodeId = nextNodeId;

	std::unique_ptr<ComputeNode> node = nodeFactory.create(typeName);
	if(node) {
		nodes[nodeId] = std::move(node);
		nextNodeId++;
	}

	return nodeId;
}
void ComputeGraph::removeNode(id_t nodeId) {
	for(auto& nodeEntry : nodes) {
		for(auto& link : nodeEntry.second->inputs) {
			if(link.nodeId == nodeId) {
				link = ComputeNode::Link();
			}
		}
	}

	nodes.erase(nodeId);
}

void ComputeGraph::linkNodes(id_t sourceNodeId, id_t targetNodeId, uint32_t sourceSlot, uint32_t targetSlot) {
	if(!hasNode(sourceNodeId) || !hasNode(targetNodeId)) {
		return;
	}

	ComputeNode& sourceNode = *(nodes.at(sourceNodeId));
	ComputeNode& targetNode = *(nodes.at(targetNodeId));
	if(targetSlot >= targetNode.inputSlots.size() || sourceSlot >= sourceNode.outputSlots.size()) {
		return;
	}

	targetNode.inputs[targetSlot] = ComputeNode::Link(
		nextLinkId++,
		sourceNodeId,
		sourceSlot);
}
void ComputeGraph::linkNodes(id_t sourceNodeId, id_t targetNodeId, const std::string& sourceSlotName, const std::string& targetSlotName) {
	if(!hasNode(sourceNodeId) || !hasNode(targetNodeId)) {
		return;
	}

	ComputeNode& sourceNode = *(nodes.at(sourceNodeId));
	ComputeNode& targetNode = *(nodes.at(targetNodeId));

	uint32_t sourceSlot = sourceNode.findOutputSlot(sourceSlotName);
	uint32_t targetSlot = targetNode.findInputSlot(targetSlotName);

	if(sourceSlot != nullId && targetSlot != nullId) {
		targetNode.inputs[targetSlot] = ComputeNode::Link(
			nextLinkId++,
			sourceNodeId,
			sourceSlot);
	}
}
void ComputeGraph::linkNodeToInput(id_t targetNodeId, uint32_t targetSlot, pixelpart::id_t inputId) {
	if(!hasNode(targetNodeId)) {
		return;
	}

	ComputeNode& targetNode = *(nodes.at(targetNodeId));
	if(targetSlot >= targetNode.inputSlots.size()) {
		return;
	}

	targetNode.inputs[targetSlot] = ComputeNode::Link(
		nextLinkId++,
		nullId,
		inputId);
}
void ComputeGraph::unlinkNodes(id_t sourceNodeId, id_t targetNodeId, uint32_t targetSlot) {
	if(!hasNode(sourceNodeId) || !hasNode(targetNodeId)) {
		return;
	}

	ComputeNode& targetNode = *(nodes.at(targetNodeId));
	if(targetSlot >= targetNode.inputs.size()) {
		return;
	}

	targetNode.inputs[targetSlot] = ComputeNode::Link();
}
void ComputeGraph::unlinkNodes(id_t linkId) {
	for(auto& nodeEntry : nodes) {
		for(ComputeNode::Link& link : nodeEntry.second->inputs) {
			if(link.id == linkId) {
				link = ComputeNode::Link();
				return;
			}
		}
	}
}
void ComputeGraph::unlinkRemovedInputs(const InputSet& graphInputs) {
	for(auto& nodeEntry : nodes) {
		for(ComputeNode::Link& link : nodeEntry.second->inputs) {
			if(link.nodeId == nullId && link.slot != nullId && graphInputs.count(link.slot) == 0u) {
				link = ComputeNode::Link();
			}
		}
	}
}

void ComputeGraph::setNodeName(id_t nodeId, const std::string& name) {
	if(!hasNode(nodeId)) {
		return;
	}

	const auto& node = nodes.at(nodeId);
	node->name = name;
}
void ComputeGraph::setNodeParameter(id_t nodeId, uint32_t parameterIndex, VariantParameter::Value value) {
	if(!hasNode(nodeId)) {
		return;
	}

	const auto& node = nodes.at(nodeId);
	node->parameterValues[parameterIndex] = value;
}
void ComputeGraph::setNodeParameter(id_t nodeId, const std::string& parameterName, VariantParameter::Value value) {
	if(!hasNode(nodeId)) {
		return;
	}

	const auto& node = nodes.at(nodeId);
	uint32_t parameterIndex = node->findParameter(parameterName);
	if(parameterIndex == nullId) {
		return;
	}

	node->parameterValues[parameterIndex] = value;
}
void ComputeGraph::setNodePosition(id_t nodeId, const vec2_t& position) {
	if(!hasNode(nodeId)) {
		return;
	}

	const auto& node = nodes.at(nodeId);
	node->position = position;
}

bool ComputeGraph::hasNode(id_t nodeId) const {
	const auto& nodeEntry = nodes.find(nodeId);
	if(nodeEntry == nodes.end()) {
		return false;
	}

	return nodeEntry->second != nullptr;
}
const ComputeNode& ComputeGraph::getNode(id_t nodeId) const {
	return *(nodes.at(nodeId));
}
const std::unordered_map<id_t, std::unique_ptr<ComputeNode>>& ComputeGraph::getNodes() const {
	return nodes;
}

id_t ComputeGraph::getNextNodeId() const {
	return nextNodeId;
}
id_t ComputeGraph::getNextLinkId() const {
	return nextLinkId;
}

bool ComputeGraph::isEmpty() const {
	return nodes.size() == 0u || (nodes.size() == 1u &&
		std::all_of(nodes.begin()->second->inputs.begin(), nodes.begin()->second->inputs.end(), [](const ComputeNode::Link& l) {
			return l.id == nullId;
		}));
}

uint32_t ComputeGraph::findNodeSignature(const InputSet& graphInputs, const BuildResult& result, const ComputeNode& node, std::vector<TypeMatch>& typeMatch) const {
	const auto matchTypes = [](VariantValue::Type type1, VariantValue::Type type2) {
		if(type1 == VariantValue::type_null || type2 == VariantValue::type_null) {
			return ComputeGraph::typematch_none;
		}
		else if(type1 == type2) {
			return ComputeGraph::typematch_exact;
		}
		else if(type1 > type2) {
			return ComputeGraph::typematch_downcast;
		}
		else if(type1 < type2 && (type1 == VariantValue::type_bool || type1 == VariantValue::type_int || type1 == VariantValue::type_float)) {
			return ComputeGraph::typematch_upcast;
		}

		return ComputeGraph::typematch_none;
	};

	uint32_t bestSignature = nullId;
	typeMatch = std::vector<TypeMatch>(node.inputs.size(), typematch_none);

	for(uint32_t s = 0u; s < static_cast<uint32_t>(node.signatures.size()); s++) {
		const ComputeNode::Signature& signature = node.signatures[s];

		std::vector<TypeMatch> currentMatch(node.inputs.size(), typematch_none);
		for(std::size_t i = 0u; i < node.inputs.size(); i++) {
			VariantValue::Type inputType = VariantValue::type_null;
			id_t sourceNodeId = node.inputs[i].nodeId;

			if(hasNode(sourceNodeId)) {
				uint32_t sourceSlot = node.inputs[i].slot;
				uint32_t sourceSignature = result.nodeSignatures.at(sourceNodeId);

				inputType = getNode(sourceNodeId).signatures[sourceSignature].outputTypes[sourceSlot];
			}
			else if(node.inputs[i].nodeId == pixelpart::nullId && graphInputs.count(node.inputs[i].slot) != 0u) {
				inputType = graphInputs.at(node.inputs[i].slot).type;
			}
			else {
				inputType = node.defaultInputs[i].type;
			}

			currentMatch[i] = matchTypes(inputType, signature.inputTypes[i]);
		}

		bool matchNotWorse = true;
		bool matchBetterForAnyInput = node.inputs.empty();
		for(uint32_t i = 0u; i < node.inputs.size(); i++) {
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

void to_json(nlohmann::ordered_json& j, const ComputeGraph& computeGraph) {
	nlohmann::ordered_json& jNodes = j["nodes"];

	for(const auto& nodeEntry : computeGraph.getNodes()) {
		nlohmann::ordered_json jNodeEntry;
		jNodeEntry.push_back(nodeEntry.first);
		jNodeEntry.push_back(*nodeEntry.second);

		jNodes.push_back(jNodeEntry);
	}
}
void from_json(const nlohmann::ordered_json& j, ComputeGraph& computeGraph) {
	std::unordered_map<id_t, std::unique_ptr<ComputeNode>> nodes;

	const nlohmann::ordered_json& jNodes = j.at("nodes");
	for(const nlohmann::ordered_json& jNodeEntry : jNodes) {
		id_t nodeId = jNodeEntry.at(0);

		const nlohmann::ordered_json& jNode = jNodeEntry.at(1);
		std::string nodeType = jNode.at("type");

		std::unique_ptr<ComputeNode> node = ComputeGraph::nodeFactory.create(nodeType);
		if(node) {
			from_json(jNode, *node);
			nodes[nodeId] = std::move(node);
		}
	}

	computeGraph = ComputeGraph(nodes);
}
}