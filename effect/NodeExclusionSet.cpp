#include "NodeExclusionSet.h"

namespace pixelpart {
NodeExclusionSet::NodeExclusionSet(const std::set<id_t>& nodeIds) : excludedNodeIds(nodeIds) {

}

NodeExclusionSet::iterator NodeExclusionSet::begin() {
	return excludedNodeIds.begin();
}
NodeExclusionSet::iterator NodeExclusionSet::end() {
	return excludedNodeIds.end();
}

NodeExclusionSet::const_iterator NodeExclusionSet::begin() const {
	return excludedNodeIds.begin();
}
NodeExclusionSet::const_iterator NodeExclusionSet::end() const {
	return excludedNodeIds.end();
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

std::size_t NodeExclusionSet::count() const {
	return excludedNodeIds.size();
}

const std::set<id_t>& NodeExclusionSet::nodeIds() const {
	return excludedNodeIds;
}

void to_json(nlohmann::ordered_json& j, const NodeExclusionSet& set) {
	j = set.nodeIds();
}
void from_json(const nlohmann::ordered_json& j, NodeExclusionSet& set) {
	set = NodeExclusionSet(j.get<std::set<id_t>>());
}
}