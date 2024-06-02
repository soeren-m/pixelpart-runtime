#pragma once

#include "Node.h"
#include <algorithm>
#include <type_traits>

namespace pixelpart {
template <typename T>
class NodeCollection {
static_assert(std::is_base_of<Node, T>::value, "T must have base class Node");

public:
	using iterator = typename std::vector<T>::iterator;
	using const_iterator = typename std::vector<T>::const_iterator;

	NodeCollection() {

	}

	iterator begin() {
		return nodes.begin();
	}
	iterator end() {
		return nodes.end();
	}
    const_iterator begin() const {
		return nodes.begin();
	}
    const_iterator end() const {
		return nodes.end();
	}

	void set(const std::vector<T>& nodeList) {
		nodes = nodeList;
		updateIndexMap();
	}

	id_t set(const T& node, id_t baseId) {
		id_t nodeId = node.id != nullId
			? baseId + node.id
			: baseId;
		id_t parentId = node.parentId != nullId
			? baseId + node.parentId
			: nullId;

		nodes.push_back(node);

		T& insertedNode = nodes.back();
		insertedNode.id = nodeId;
		insertedNode.parentId = parentId;
		updateIndexMap();

		return nodeId;
	}

	id_t add(const T& node) {
		id_t nodeId = node.id != nullId ? node.id : 0u;
		while(contains(nodeId)) {
			nodeId++;
		}

		nodes.push_back(node);
		nodes.back().id = nodeId;
		updateIndexMap();

		return nodeId;
	}

	id_t duplicate(id_t nodeId, const std::string& nameExtension = " (copy)") {
		if(!contains(nodeId)) {
			return nullId;
		}

		T otherNode = get(nodeId);
		otherNode.id = nullId;
		otherNode.parentId = nullId;

		do {
			otherNode.name += nameExtension;
		}
		while(containsName(otherNode.name));

		return add(otherNode);
	}

	void remove(id_t nodeId) {
		removeAt(findById(nodeId));
	}
	void removeAt(uint32_t index) {
		if(index >= nodes.size()) {
			return;
		}

		nodes.erase(nodes.begin() + index);
		updateIndexMap();
	}
	void removeAll() {
		nodes.clear();
		updateIndexMap();
	}

	uint32_t findById(id_t nodeId) const {
		return (nodeId < indexMap.size())
			? indexMap[nodeId]
			: nullId;
	}
	uint32_t findByParent(id_t parentId) const {
		for(uint32_t i = 0u; i < nodes.size(); i++) {
			if(nodes[i].parentId == parentId) {
				return i;
			}
		}

		return nullId;
	}
	uint32_t findByName(const std::string& name) const {
		for(uint32_t i = 0u; i < nodes.size(); i++) {
			if(nodes[i].name == name) {
				return i;
			}
		}

		return nullId;
	}

	bool contains(id_t nodeId) const {
		return findById(nodeId) != nullId;
	}
	bool containsParent(id_t parentId) const {
		return findByParent(parentId) != nullId;
	}
	bool containsName(const std::string& name) const {
		return findByName(name) != nullId;
	}
	bool containsIndex(uint32_t index) const {
		return index < nodes.size();
	}

	T& get(id_t nodeId) {
		return nodes.at(findById(nodeId));
	}
	T& getByParent(id_t parentId) {
		return nodes.at(findByParent(parentId));
	}
	T& getByName(const std::string& name) {
		return nodes.at(findByName(name));
	}
	T& getByIndex(uint32_t index) {
		return nodes.at(index);
	}
	const std::vector<T>& get() const {
		return nodes;
	}
	const T& get(id_t id) const {
		return nodes.at(findById(id));
	}
	const T& getByParent(id_t parentId) const {
		return nodes.at(findByParent(parentId));
	}
	const T& getByName(const std::string& name) const {
		return nodes.at(findByName(name));
	}
	const T& getByIndex(uint32_t index) const {
		return nodes.at(index);
	}

	uint32_t getCount() const {
		return static_cast<uint32_t>(nodes.size());
	}
	id_t getMaxId() const {
		id_t maxId = 0u;
		for(const T& node : nodes) {
			maxId = std::max(maxId, node.id);
		}

		return maxId;
	}

	template <typename TFunc>
	std::vector<T> getSorted(TFunc compare) const {
		std::vector<T> sortedNodes = nodes;
		std::sort(sortedNodes.begin(), sortedNodes.end(), [this, &compare](const T& node1, const T& node2) {
			return compare(node1, node2);
		});

		return sortedNodes;
	}

	template <typename TFunc>
	std::vector<uint32_t> getSortedIndices(TFunc compare) const {
		std::vector<uint32_t> sortedIndices(nodes.size());
		for(uint32_t i = 0u; i < nodes.size(); i++) {
			sortedIndices[i] = i;
		}

		std::sort(sortedIndices.begin(), sortedIndices.end(), [this, &compare](uint32_t index1, uint32_t index2) {
			return compare(nodes[index1], nodes[index2]);
		});

		return sortedIndices;
	}

private:
	void updateIndexMap() {
		indexMap.clear();
		indexMap.reserve(nodes.size());

		for(uint32_t i = 0u; i < nodes.size(); i++) {
			id_t nodeId = nodes[i].id;
			if(nodeId >= indexMap.size()) {
				indexMap.resize(nodeId + 1u, nullId);
			}

			indexMap[nodeId] = i;
		}
	}

	std::vector<T> nodes;
	std::vector<uint32_t> indexMap;
};
}