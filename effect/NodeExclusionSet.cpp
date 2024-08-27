#include "NodeExclusionSet.h"

namespace pixelpart {
NodeExclusionSet::NodeExclusionSet(const std::unordered_set<id_t>& nodeIds) : excludedNodeIds(nodeIds) {

}

void NodeExclusionSet::add(id_t nodeId) {
	excludedNodeIds.insert(nodeId);
}
void NodeExclusionSet::add(const Node& node) {
	add(node.id());
}

void NodeExclusionSet::remove(id_t nodeId) {
	excludedNodeIds.erase(nodeId);
}
void NodeExclusionSet::remove(const Node& node) {
	remove(node.id());
}

bool NodeExclusionSet::excluded(id_t nodeId) const {
	return excludedNodeIds.count(nodeId) != 0;
}
bool NodeExclusionSet::excluded(const Node& node) const {
	return excluded(node.id());
}

const std::unordered_set<id_t>& NodeExclusionSet::nodeIds() const {
	return excludedNodeIds;
}

void to_json(nlohmann::ordered_json& j, const NodeExclusionSet& set) {
	j = set.nodeIds();
}
void from_json(const nlohmann::ordered_json& j, NodeExclusionSet& set) {
	set = NodeExclusionSet(j.get<std::unordered_set<id_t>>());
}
}