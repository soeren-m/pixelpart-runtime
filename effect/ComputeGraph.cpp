#include "ComputeGraph.h"
#include <algorithm>

namespace pixelpart {
ComputeGraph::EvaluationException::EvaluationException(const char* msg, id_t node, std::optional<std::uint32_t> slot) :
	std::runtime_error(msg), exceptionNodeId(node), exceptionSlotIndex(slot) {

}
ComputeGraph::EvaluationException::EvaluationException(const std::string& msg, id_t node, std::optional<std::uint32_t> slot) :
	std::runtime_error(msg), exceptionNodeId(node), exceptionSlotIndex(slot) {

}

id_t ComputeGraph::EvaluationException::nodeId() const {
	return exceptionNodeId;
}
std::optional<std::uint32_t> ComputeGraph::EvaluationException::slotIndex() const {
	return exceptionSlotIndex;
}

ComputeNodeFactory ComputeGraph::nodeFactory = ComputeNodeFactory();

ComputeGraph::ComputeGraph(const ComputeGraph& other) {
	for(const auto& [nodeId, node] : other.graphNodes) {
		if(node) {
			graphNodes[nodeId] = std::move(node->clone());
		}
	}

	graphNextNodeId = other.graphNextNodeId;
	graphNextLinkId = other.graphNextLinkId;
}
ComputeGraph::ComputeGraph(const ComputeNodeCollection& initialNodes) {
	for(const auto& [nodeId, node] : initialNodes) {
		if(node) {
			graphNodes[nodeId] = std::move(node->clone());
		}
	}

	id_t maxNodeId = 0;
	id_t maxLinkId = 0;

	for(const auto& [nodeId, node] : graphNodes) {
		maxNodeId = std::max(maxNodeId, nodeId);

		for(const auto& link : node->inputLinks()) {
			if(!link.id) {
				continue;
			}

			maxLinkId = std::max(maxLinkId, link.id);
		}
	}

	graphNextNodeId = maxNodeId.value() + 1;
	graphNextLinkId = maxLinkId.value() + 1;
}

ComputeGraph& ComputeGraph::operator=(const ComputeGraph& other) {
	graphNodes.clear();
	for(const auto& [nodeId, node] : other.graphNodes) {
		if(node) {
			graphNodes[nodeId] = std::move(node->clone());
		}
	}

	graphNextNodeId = other.graphNextNodeId;
	graphNextLinkId = other.graphNextLinkId;

	return *this;
}

std::vector<VariantValue> ComputeGraph::evaluate(const InputSet& graphInputs) const {
	BuildResult buildResult;

	return evaluate(graphInputs, buildResult);
}
std::vector<VariantValue> ComputeGraph::evaluate(const InputSet& graphInputs, BuildResult& result, id_t nodeId) const {
	auto nodeIt = graphNodes.find(nodeId);
	if(nodeIt == graphNodes.end()) {
		return std::vector<VariantValue>();
	}

	const auto& activeNode = nodeIt->second;
	std::vector<VariantValue> in(activeNode->inputLinks().size());

	for(std::uint32_t inputSlot = 0; inputSlot < activeNode->inputLinks().size(); inputSlot++) {
		const ComputeNode::Link& link = activeNode->inputLinks()[inputSlot];

		VariantValue::Type sourceValueType = VariantValue::type_null;
		VariantValue inputValue;

		if(link.id && containsNode(link.nodeId)) {
			if(result.nodeOutputs.count(link.nodeId) == 0) {
				result.nodeOutputs[link.nodeId] = evaluate(graphInputs, result, link.nodeId);
			}

			if(link.slot >= result.nodeOutputs[link.nodeId].size()) {
				throw EvaluationException("Node links to non-existent slot", nodeId, inputSlot);
			}

			const ComputeNode& sourceNode = node(link.nodeId);
			const ComputeNode::Signature& sourceNodeSignature = sourceNode.signatures()[result.nodeSignatures[link.nodeId]];

			sourceValueType = sourceNodeSignature.outputTypes[link.slot];
			inputValue = result.nodeOutputs[link.nodeId][link.slot];
		}
		else if(link.id && !link.nodeId && graphInputs.count(link.slot) != 0) {
			sourceValueType = graphInputs.at(link.slot).type();
			inputValue = graphInputs.at(link.slot);
		}
		else if(inputSlot < activeNode->defaultInputs().size() && activeNode->defaultInputs()[inputSlot].type() != VariantValue::type_null) {
			sourceValueType = activeNode->defaultInputs()[inputSlot].type();
			inputValue = activeNode->defaultInputs()[inputSlot];
		}
		else {
			throw EvaluationException("Input node not found", nodeId, inputSlot);
		}

		in[inputSlot] = inputValue.cast(sourceValueType);
	}

	std::vector<TypeMatch> typeMatch;
	std::optional<std::uint32_t> signatureIndex = findNodeSignature(graphInputs, result, *activeNode, typeMatch);
	if(!signatureIndex) {
		throw EvaluationException("No matching node signature found", nodeId);
	}

	result.nodeSignatures[nodeId] = signatureIndex.value();

	for(std::uint32_t inputSlot = 0; inputSlot < activeNode->inputLinks().size(); inputSlot++) {
		if(typeMatch[inputSlot] == TypeMatch::none) {
			throw EvaluationException("Types do not match", nodeId, inputSlot);
		}
	}

	return activeNode->evaluate(in);
}

id_t ComputeGraph::addNode(const std::string& typeName) {
	id_t nodeId = graphNextNodeId;

	std::unique_ptr<ComputeNode> node = nodeFactory.createNode(typeName);
	if(node) {
		graphNodes[nodeId] = std::move(node);
		graphNextNodeId++;
	}

	return nodeId;
}
void ComputeGraph::removeNode(id_t nodeId) {
	for(auto& [otherNodeId, otherNode] : graphNodes) {
		for(auto& link : otherNode->inputLinks()) {
			if(link.nodeId == nodeId) {
				link = ComputeNode::Link();
			}
		}
	}

	graphNodes.erase(nodeId);
}
bool ComputeGraph::containsNode(id_t nodeId) const {
	auto nodeIt = graphNodes.find(nodeId);
	if(nodeIt == graphNodes.end()) {
		return false;
	}

	return nodeIt->second != nullptr;
}

void ComputeGraph::linkNodes(id_t sourceNodeId, id_t targetNodeId, std::uint32_t sourceSlot, std::uint32_t targetSlot) {
	if(!containsNode(sourceNodeId) || !containsNode(targetNodeId)) {
		return;
	}

	auto& sourceNode = graphNodes.at(sourceNodeId);
	auto& targetNode = graphNodes.at(targetNodeId);
	if(targetSlot >= targetNode->inputSlots().size() || sourceSlot >= sourceNode->outputSlots().size()) {
		return;
	}

	targetNode->inputLinks()[targetSlot] = ComputeNode::Link(
		graphNextLinkId++,
		sourceNodeId,
		sourceSlot);
}
void ComputeGraph::linkNodes(id_t sourceNodeId, id_t targetNodeId, const std::string& sourceSlotName, const std::string& targetSlotName) {
	if(!containsNode(sourceNodeId) || !containsNode(targetNodeId)) {
		return;
	}

	auto& sourceNode = graphNodes.at(sourceNodeId);
	auto& targetNode = graphNodes.at(targetNodeId);

	std::optional<std::uint32_t> sourceSlot = sourceNode->findOutputSlot(sourceSlotName);
	std::optional<std::uint32_t> targetSlot = targetNode->findInputSlot(targetSlotName);

	if(sourceSlot && targetSlot) {
		targetNode->inputLinks()[targetSlot.value()] = ComputeNode::Link(
			graphNextLinkId++,
			sourceNodeId,
			sourceSlot.value());
	}
}
void ComputeGraph::linkNodeToInput(id_t targetNodeId, std::uint32_t targetSlot, id_t inputId) {
	if(!containsNode(targetNodeId)) {
		return;
	}

	auto& targetNode = graphNodes.at(targetNodeId);
	if(targetSlot >= targetNode->inputSlots().size()) {
		return;
	}

	targetNode->inputLinks()[targetSlot] = ComputeNode::Link(
		graphNextLinkId++,
		id_t(),
		inputId.value());
}
void ComputeGraph::unlinkNodes(id_t sourceNodeId, id_t targetNodeId, std::uint32_t targetSlot) {
	if(!containsNode(sourceNodeId) || !containsNode(targetNodeId)) {
		return;
	}

	auto& targetNode = graphNodes.at(targetNodeId);
	if(targetSlot >= targetNode->inputLinks().size()) {
		return;
	}

	targetNode->inputLinks()[targetSlot] = ComputeNode::Link();
}
void ComputeGraph::unlinkNodes(id_t linkId) {
	for(auto& [nodeId, node] : graphNodes) {
		for(auto& link : node->inputLinks()) {
			if(link.id == linkId) {
				link = ComputeNode::Link();
				return;
			}
		}
	}
}
void ComputeGraph::unlinkRemovedInputs(const InputSet& graphInputs) {
	for(auto& [nodeId, node] : graphNodes) {
		for(auto& link : node->inputLinks()) {
			if(link.id && !link.nodeId && graphInputs.count(link.slot) == 0) {
				link = ComputeNode::Link();
			}
		}
	}
}

void ComputeGraph::nodeName(id_t nodeId, const std::string& name) {
	if(!containsNode(nodeId)) {
		return;
	}

	auto& node = graphNodes.at(nodeId);
	node->name(name);
}
void ComputeGraph::nodeParameter(id_t nodeId, std::uint32_t parameterIndex, VariantParameter::Value value) {
	if(!containsNode(nodeId)) {
		return;
	}

	auto& node = graphNodes.at(nodeId);
	node->parameterValue(parameterIndex) = value;
}
void ComputeGraph::nodeParameter(id_t nodeId, const std::string& parameterName, VariantParameter::Value value) {
	if(!containsNode(nodeId)) {
		return;
	}

	auto& node = graphNodes.at(nodeId);

	std::optional<std::uint32_t> parameterIndex = node->findParameter(parameterName);
	if(!parameterIndex) {
		return;
	}

	node->parameterValue(parameterIndex.value()) = value;
}
void ComputeGraph::nodePosition(id_t nodeId, const float2_t& position) {
	if(!containsNode(nodeId)) {
		return;
	}

	auto& node = graphNodes.at(nodeId);
	node->move(position);
}

const ComputeNode& ComputeGraph::node(id_t nodeId) const {
	return *(graphNodes.at(nodeId));
}
const ComputeGraph::ComputeNodeCollection& ComputeGraph::nodes() const {
	return graphNodes;
}

bool ComputeGraph::empty() const {
	return graphNodes.empty() || (graphNodes.size() == 1 &&
		std::all_of(graphNodes.begin()->second->inputLinks().begin(), graphNodes.begin()->second->inputLinks().end(), [](const ComputeNode::Link& l) {
			return !l.id;
		}));
}

std::optional<std::uint32_t> ComputeGraph::findNodeSignature(const InputSet& graphInputs, const BuildResult& result, const ComputeNode& activeNode, std::vector<TypeMatch>& typeMatch) const {
	const auto matchTypes = [](VariantValue::Type type1, VariantValue::Type type2) {
		if(type1 == VariantValue::type_null || type2 == VariantValue::type_null) {
			return TypeMatch::none;
		}
		else if(type1 == type2) {
			return TypeMatch::exact;
		}
		else if(type1 > type2) {
			return TypeMatch::downcast;
		}
		else if(type1 < type2 && (type1 == VariantValue::type_bool || type1 == VariantValue::type_int || type1 == VariantValue::type_float)) {
			return TypeMatch::upcast;
		}

		return TypeMatch::none;
	};

	std::optional<std::uint32_t> bestSignature;
	typeMatch = std::vector<TypeMatch>(activeNode.inputLinks().size(), TypeMatch::none);

	for(std::size_t signatureIndex = 0; signatureIndex < activeNode.signatures().size(); signatureIndex++) {
		const ComputeNode::Signature& signature = activeNode.signatures()[signatureIndex];

		std::vector<TypeMatch> currentMatch(activeNode.inputLinks().size(), TypeMatch::none);
		for(std::size_t inputIndex = 0; inputIndex < activeNode.inputLinks().size(); inputIndex++) {
			VariantValue::Type inputType = VariantValue::type_null;
			id_t sourceNodeId = activeNode.inputLinks()[inputIndex].nodeId;

			if(containsNode(sourceNodeId)) {
				std::uint32_t sourceSlot = activeNode.inputLinks()[inputIndex].slot;
				std::uint32_t sourceSignature = result.nodeSignatures.at(sourceNodeId);

				inputType = node(sourceNodeId).signatures()[sourceSignature].outputTypes[sourceSlot];
			}
			else if(!activeNode.inputLinks()[inputIndex].nodeId && graphInputs.count(activeNode.inputLinks()[inputIndex].slot) != 0) {
				inputType = graphInputs.at(activeNode.inputLinks()[inputIndex].slot).type();
			}
			else {
				inputType = activeNode.defaultInputs()[inputIndex].type();
			}

			currentMatch[inputIndex] = matchTypes(inputType, signature.inputTypes[inputIndex]);
		}

		bool matchNotWorse = true;
		bool matchBetterForAnyInput = activeNode.inputLinks().empty();
		for(std::size_t inputIndex = 0; inputIndex < activeNode.inputLinks().size(); inputIndex++) {
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

void to_json(nlohmann::ordered_json& j, const ComputeGraph& computeGraph) {
	std::vector<id_t> nodeIds;
	nodeIds.reserve(computeGraph.nodes().size());
	for(const auto& [nodeId, node] : computeGraph.nodes()) {
		nodeIds.push_back(nodeId);
	}

	std::sort(nodeIds.begin(), nodeIds.end());

	nlohmann::ordered_json& jNodes = j["nodes"];

	for(id_t nodeId : nodeIds) {
		nlohmann::ordered_json jNodeEntry;
		jNodeEntry.push_back(nodeId);
		jNodeEntry.push_back(computeGraph.node(nodeId));

		jNodes.push_back(jNodeEntry);
	}
}
void from_json(const nlohmann::ordered_json& j, ComputeGraph& computeGraph) {
	if(!j.contains("nodes")) {
		computeGraph = ComputeGraph();
		return;
	}

	ComputeGraph::ComputeNodeCollection nodes;
	for(const nlohmann::ordered_json& jNodeEntry : j.at("nodes")) {
		id_t nodeId = jNodeEntry.at(0);

		const nlohmann::ordered_json& jNode = jNodeEntry.at(1);
		std::string nodeType = jNode.at("type");

		std::unique_ptr<ComputeNode> node = ComputeGraph::nodeFactory.createNode(nodeType);
		if(node) {
			from_json(jNode, *node);
			nodes[nodeId] = std::move(node);
		}
	}

	computeGraph = ComputeGraph(nodes);
}
}
