#include "ComputeGraph.h"
#include <algorithm>

namespace pixelpart {
ComputeGraph::EvaluationException::EvaluationException(const std::string& msg, id_t node, std::uint32_t slot) :
	std::runtime_error(msg), computeNodeId(node), computeSlotIndex(slot) {

}

id_t ComputeGraph::EvaluationException::nodeId() const {
	return computeNodeId;
}
std::uint32_t ComputeGraph::EvaluationException::slotIndex() const {
	return computeSlotIndex;
}

ComputeNodeFactory ComputeGraph::nodeFactory = ComputeNodeFactory();

ComputeGraph::ComputeGraph(const ComputeGraph& other) {
	for(const auto& node : other.computeNodes) {
		if(node.second) {
			computeNodes[node.first] = std::move(node.second->clone());
		}
	}

	nextNodeId = other.nextNodeId;
	nextLinkId = other.nextLinkId;
}
ComputeGraph::ComputeGraph(const ComputeNodeCollection& initialNodes) {
	for(const auto& node : initialNodes) {
		if(node.second) {
			computeNodes[node.first] = std::move(node.second->clone());
		}
	}

	id_t maxNodeId = 0;
	id_t maxLinkId = 0;

	for(const auto& node : computeNodes) {
		maxNodeId = std::max(maxNodeId, node.first);

		for(const auto& link : node.second->inputLinks()) {
			if(!link.id) {
				continue;
			}

			maxLinkId = std::max(maxLinkId, link.id);
		}
	}

	nextNodeId = maxNodeId.value() + 1;
	nextLinkId = maxLinkId.value() + 1;
}

ComputeGraph& ComputeGraph::operator=(const ComputeGraph& other) {
	computeNodes.clear();
	for(const auto& node : other.computeNodes) {
		if(node.second) {
			computeNodes[node.first] = std::move(node.second->clone());
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
	const auto& nodeEntry = computeNodes.find(nodeId);
	if(nodeEntry == computeNodes.end()) {
		return std::vector<VariantValue>();
	}

	const std::unique_ptr<ComputeNode>& activeNode = nodeEntry->second;
	std::vector<VariantValue> in(activeNode->inputLinks().size());

	for(std::uint32_t inputSlot = 0; inputSlot < activeNode->inputLinks().size(); inputSlot++) {
		const ComputeNode::Link& link = activeNode->inputLinks()[inputSlot];

		VariantValue::Type sourceValueType = VariantValue::type_null;
		VariantValue inputValue;

		if(containsNode(link.nodeId)) {
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
		else if(!link.nodeId && graphInputs.count(link.slot) != 0) {
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
	result.nodeSignatures[nodeId] = findNodeSignature(graphInputs, result, *activeNode, typeMatch);
	if(result.nodeSignatures[nodeId] == id_t::nullValue) {
		throw EvaluationException("No matching node signature found", nodeId);
	}

	for(std::uint32_t inputSlot = 0; inputSlot < activeNode->inputLinks().size(); inputSlot++) {
		if(typeMatch[inputSlot] == typematch_none) {
			throw EvaluationException("Types do not match", nodeId, inputSlot);
		}
	}

	return activeNode->evaluate(in);
}

id_t ComputeGraph::addNode(const std::string& typeName) {
	id_t nodeId = nextNodeId;

	std::unique_ptr<ComputeNode> node = nodeFactory.create(typeName);
	if(node) {
		computeNodes[nodeId] = std::move(node);
		nextNodeId++;
	}

	return nodeId;
}
void ComputeGraph::removeNode(id_t nodeId) {
	for(auto& nodeEntry : computeNodes) {
		for(auto& link : nodeEntry.second->inputLinks()) {
			if(link.nodeId == nodeId) {
				link = ComputeNode::Link();
			}
		}
	}

	computeNodes.erase(nodeId);
}
bool ComputeGraph::containsNode(id_t nodeId) const {
	const auto& nodeEntry = computeNodes.find(nodeId);
	if(nodeEntry == computeNodes.end()) {
		return false;
	}

	return nodeEntry->second != nullptr;
}

void ComputeGraph::linkNodes(id_t sourceNodeId, id_t targetNodeId, std::uint32_t sourceSlot, std::uint32_t targetSlot) {
	if(!containsNode(sourceNodeId) || !containsNode(targetNodeId)) {
		return;
	}

	ComputeNode& sourceNode = *(computeNodes.at(sourceNodeId));
	ComputeNode& targetNode = *(computeNodes.at(targetNodeId));
	if(targetSlot >= targetNode.inputSlots().size() || sourceSlot >= sourceNode.outputSlots().size()) {
		return;
	}

	targetNode.inputLinks()[targetSlot] = ComputeNode::Link(
		nextLinkId++,
		sourceNodeId,
		sourceSlot);
}
void ComputeGraph::linkNodes(id_t sourceNodeId, id_t targetNodeId, const std::string& sourceSlotName, const std::string& targetSlotName) {
	if(!containsNode(sourceNodeId) || !containsNode(targetNodeId)) {
		return;
	}

	ComputeNode& sourceNode = *(computeNodes.at(sourceNodeId));
	ComputeNode& targetNode = *(computeNodes.at(targetNodeId));

	std::uint32_t sourceSlot = sourceNode.findOutputSlot(sourceSlotName);
	std::uint32_t targetSlot = targetNode.findInputSlot(targetSlotName);

	if(sourceSlot != id_t::nullValue && targetSlot != id_t::nullValue) {
		targetNode.inputLinks()[targetSlot] = ComputeNode::Link(
			nextLinkId++,
			sourceNodeId,
			sourceSlot);
	}
}
void ComputeGraph::linkNodeToInput(id_t targetNodeId, std::uint32_t targetSlot, id_t inputId) {
	if(!containsNode(targetNodeId)) {
		return;
	}

	ComputeNode& targetNode = *(computeNodes.at(targetNodeId));
	if(targetSlot >= targetNode.inputSlots().size()) {
		return;
	}

	targetNode.inputLinks()[targetSlot] = ComputeNode::Link(
		nextLinkId++,
		id_t(),
		static_cast<std::uint32_t>(inputId));
}
void ComputeGraph::unlinkNodes(id_t sourceNodeId, id_t targetNodeId, std::uint32_t targetSlot) {
	if(!containsNode(sourceNodeId) || !containsNode(targetNodeId)) {
		return;
	}

	ComputeNode& targetNode = *(computeNodes.at(targetNodeId));
	if(targetSlot >= targetNode.inputLinks().size()) {
		return;
	}

	targetNode.inputLinks()[targetSlot] = ComputeNode::Link();
}
void ComputeGraph::unlinkNodes(id_t linkId) {
	for(auto& nodeEntry : computeNodes) {
		for(ComputeNode::Link& link : nodeEntry.second->inputLinks()) {
			if(link.id == linkId) {
				link = ComputeNode::Link();
				return;
			}
		}
	}
}
void ComputeGraph::unlinkRemovedInputs(const InputSet& graphInputs) {
	for(auto& nodeEntry : computeNodes) {
		for(ComputeNode::Link& link : nodeEntry.second->inputLinks()) {
			if(!link.nodeId && link.slot != id_t::nullValue && graphInputs.count(link.slot) == 0) {
				link = ComputeNode::Link();
			}
		}
	}
}

void ComputeGraph::nodeName(id_t nodeId, const std::string& name) {
	if(!containsNode(nodeId)) {
		return;
	}

	auto& node = computeNodes.at(nodeId);
	node->name(name);
}
void ComputeGraph::nodeParameter(id_t nodeId, std::uint32_t parameterIndex, VariantParameter::Value value) {
	if(!containsNode(nodeId)) {
		return;
	}

	auto& node = computeNodes.at(nodeId);
	node->parameterValues()[parameterIndex] = value;
}
void ComputeGraph::nodeParameter(id_t nodeId, const std::string& parameterName, VariantParameter::Value value) {
	if(!containsNode(nodeId)) {
		return;
	}

	auto& node = computeNodes.at(nodeId);
	std::uint32_t parameterIndex = node->findParameter(parameterName);
	if(parameterIndex == id_t::nullValue) {
		return;
	}

	node->parameterValues()[parameterIndex] = value;
}
void ComputeGraph::nodePosition(id_t nodeId, const float2_t& position) {
	if(!containsNode(nodeId)) {
		return;
	}

	auto& node = computeNodes.at(nodeId);
	node->move(position);
}

const ComputeNode& ComputeGraph::node(id_t nodeId) const {
	return *(computeNodes.at(nodeId));
}
const ComputeGraph::ComputeNodeCollection& ComputeGraph::nodes() const {
	return computeNodes;
}

bool ComputeGraph::empty() const {
	return computeNodes.size() == 0 || (computeNodes.size() == 1 &&
		std::all_of(computeNodes.begin()->second->inputLinks().begin(), computeNodes.begin()->second->inputLinks().end(), [](const ComputeNode::Link& l) {
			return !l.id;
		}));
}

std::uint32_t ComputeGraph::findNodeSignature(const InputSet& graphInputs, const BuildResult& result, const ComputeNode& activeNode, std::vector<TypeMatch>& typeMatch) const {
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

	std::uint32_t bestSignature = id_t::nullValue;
	typeMatch = std::vector<TypeMatch>(activeNode.inputLinks().size(), typematch_none);

	for(std::size_t signatureIndex = 0; signatureIndex < activeNode.signatures().size(); signatureIndex++) {
		const ComputeNode::Signature& signature = activeNode.signatures()[signatureIndex];

		std::vector<TypeMatch> currentMatch(activeNode.inputLinks().size(), typematch_none);
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

void to_json(nlohmann::ordered_json& j, const ComputeGraph& computeGraph) {
	std::vector<id_t> ids;
	ids.reserve(computeGraph.nodes().size());
	for(const auto& entry : computeGraph.nodes()) {
		ids.push_back(entry.first);
	}

	std::sort(ids.begin(), ids.end());

	nlohmann::ordered_json& jNodes = j["nodes"];

	for(id_t id : ids) {
		nlohmann::ordered_json jNodeEntry;
		jNodeEntry.push_back(id);
		jNodeEntry.push_back(computeGraph.node(id));

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

		std::unique_ptr<ComputeNode> node = ComputeGraph::nodeFactory.create(nodeType);
		if(node) {
			from_json(jNode, *node);
			nodes[nodeId] = std::move(node);
		}
	}

	computeGraph = ComputeGraph(nodes);
}
}
