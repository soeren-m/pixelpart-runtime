#include "NodeSet.h"

namespace pixelpart {
NodeSet::NodeSet(const std::set<id_t>& nodeIds) : ids(nodeIds) {

}

NodeSet::iterator NodeSet::begin() {
	return ids.begin();
}
NodeSet::iterator NodeSet::end() {
	return ids.end();
}

NodeSet::const_iterator NodeSet::begin() const {
	return ids.begin();
}
NodeSet::const_iterator NodeSet::end() const {
	return ids.end();
}

void NodeSet::add(id_t nodeId) {
	ids.insert(nodeId);
}
void NodeSet::add(const Node& node) {
	add(node.id());
}

void NodeSet::remove(id_t nodeId) {
	ids.erase(nodeId);
}
void NodeSet::remove(const Node& node) {
	remove(node.id());
}

bool NodeSet::excluded(id_t nodeId) const {
	return ids.count(nodeId) != 0;
}
bool NodeSet::excluded(const Node& node) const {
	return excluded(node.id());
}

std::size_t NodeSet::count() const {
	return ids.size();
}

const std::set<id_t>& NodeSet::nodeIds() const {
	return ids;
}

void to_json(nlohmann::ordered_json& j, const NodeSet& set) {
	j = set.nodeIds();
}
void from_json(const nlohmann::ordered_json& j, NodeSet& set) {
	set = NodeSet(j.get<std::set<id_t>>());
}
}