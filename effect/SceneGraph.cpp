#include "SceneGraph.h"
#include <algorithm>

namespace pixelpart {
SceneGraph::iterator SceneGraph::begin() {
	return sceneNodes.begin();
}
SceneGraph::iterator SceneGraph::end() {
	return sceneNodes.end();
}
SceneGraph::const_iterator SceneGraph::begin() const {
	return sceneNodes.begin();
}
SceneGraph::const_iterator SceneGraph::end() const {
	return sceneNodes.end();
}

void SceneGraph::set(const std::vector<std::unique_ptr<Node>>& nodeList) {

}
id_t SceneGraph::set(std::unique_ptr<Node> node, id_t baseId) {

}
id_t SceneGraph::add(std::unique_ptr<Node> node){
	if(!node) {
		return id_t();
	}

	id_t nodeId = node->nodeId ? node->nodeId : id_t(0u);
	while(contains(nodeId)) {
		nodeId++;
	}

	sceneNodes.push_back(node);
	sceneNodes.back()->nodeId = nodeId;
	updateIndexMap();

	return nodeId;
}
id_t SceneGraph::duplicate(id_t nodeId, const std::string& nameExtension = " (copy)") {

}

void SceneGraph::remove(id_t nodeId) {
	removeAt(indexOf(nodeId));
}
void SceneGraph::removeAt(uint32_t index) {
	if(index >= sceneNodes.size()) {
		return;
	}

	sceneNodes.erase(sceneNodes.begin() + index);
	updateIndexMap();
}
void SceneGraph::removeAll() {
	sceneNodes.clear();
	updateIndexMap();
}

uint32_t SceneGraph::indexOf(id_t nodeId) const {
	return nodeId.value() < indexMap.size()
		? indexMap[nodeId.value()]
		: id_t::nullValue;
}
uint32_t SceneGraph::indexOfName(const std::string& name) const {

}

bool SceneGraph::contains(id_t nodeId) const {
	return indexOf(nodeId) != id_t::nullValue;
}
bool SceneGraph::containsIndex(uint32_t index) const {
	return index < sceneNodes.size();
}

Node& SceneGraph::at(id_t nodeId) {
	return *sceneNodes.at(indexOf(nodeId));
}
Node& SceneGraph::atIndex(uint32_t index) {
	return *sceneNodes.at(index);
}
const Node& SceneGraph::at(id_t nodeId) const {
	return *sceneNodes.at(indexOf(nodeId));
}
const Node& SceneGraph::atIndex(uint32_t index) const {
	return *sceneNodes.at(index);
}

NodeTransform SceneGraph::localTransform(const Node& node, float_t t) const {

}
NodeTransform SceneGraph::localTransform(id_t nodeId, float_t t) const {

}

NodeTransform SceneGraph::globalTransform(id_t nodeId, float_t t) const {
	const Node& node = *sceneNodes.at(indexOf(nodeId));

	NodeTransform parentTransform;
	if(node.nodeParentId) {
		parentTransform = globalTransform(node.nodeParentId, t);
	}

	return parentTransform * localTransform(nodeId);
}

uint32_t SceneGraph::count() const {
	return static_cast<uint32_t>(sceneNodes.size());
}
id_t SceneGraph::maxId() const {
	id_t maxId = 0u;
	for(const std::unique_ptr<Node>& node : sceneNodes) {
		maxId = std::max(maxId, node->nodeId);
	}

	return maxId;
}

const std::vector<std::unique_ptr<Node>>& SceneGraph::nodes() const {
	return sceneNodes;
}
}