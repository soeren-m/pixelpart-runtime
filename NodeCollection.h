#pragma once

#include "Node.h"
#include <type_traits>
#include <vector>
#include <algorithm>

namespace pixelpart {
template <typename T>
class NodeCollection {
static_assert(std::is_base_of<Node, T>::value);

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

	uint32_t add(const T& node, uint32_t offsetId = 0) {
		uint32_t nodeId = offsetId + ((node.id != NullId) ? node.id : 0);
		while(contains(nodeId)) {
			nodeId++;
			offsetId++;
		}

		nodes.push_back(node);

		T& insertedNode = nodes.back();
		insertedNode.id = nodeId;
		if(insertedNode.parentId != NullId) {
			insertedNode.parentId += offsetId;
		}

		updateIndexMap();

		return nodeId;
	}

	uint32_t duplicate(uint32_t nodeId) {
		if(!contains(nodeId)) {
			return NullId;
		}

		T otherNode = get(nodeId);
		otherNode.id = NullId;
		otherNode.parentId = NullId;

		do {
			otherNode.name += "(copy)";
		} while(containsWithName(otherNode.name));

		return add(otherNode);
	}

	void remove(uint32_t nodeId) {
		removeAt(findById(nodeId));
	}
	void removeAt(uint32_t index) {
		if(index >= nodes.size()) {
			return;
		}

		uint32_t nodeId = nodes[index].id;
		for(T& node : nodes) {
			if(node.id == nodeId) {
				continue;
			}

			if(node.parentId == nodeId) {
				node.parentId = nodes[index].parentId;
			}
		}

		nodes.erase(nodes.begin() + index);
		updateIndexMap();
	}
	void removeAll() {
		nodes.clear();
		updateIndexMap();
	}

	void setParent(uint32_t nodeId, uint32_t parentId) {
		if(!contains(nodeId)) {
			return;
		}

		T& node = get(nodeId);

		if(node.parentId != parentId) {
			if(isDescendent(parentId, nodeId)) {
				for(Node& otherNode : nodes) {
					if(otherNode.parentId == nodeId) {
						otherNode.parentId = node.parentId;
					}
				}
			}

			node.parentId = parentId;
		}
	}

	bool isDescendent(uint32_t nodeId, uint32_t ancestorId) const {
		uint32_t currentId = nodeId;
		while(contains(currentId)) {
			const T& node = get(currentId);
			if(node.parentId == ancestorId) {
				return true;
			}

			currentId = node.parentId;
		}

		return false;
	}

	uint32_t findById(uint32_t nodeId) const {
		return (nodeId < indexMap.size()) 
			? indexMap[nodeId]
			: NullId;
	}
	uint32_t findByParent(uint32_t parentId) const {
		for(uint32_t i = 0; i < nodes.size(); i++) {
			if(nodes[i].parentId == parentId) {
				return i;
			}
		}

		return NullId;
	}
	uint32_t findByName(const std::string& name) const {
		for(uint32_t i = 0; i < nodes.size(); i++) {
			if(nodes[i].name == name) {
				return i;
			}
		}

		return NullId;
	}

	bool contains(uint32_t nodeId) const {
		return findById(nodeId) != NullId;
	}
	bool containsWithParent(uint32_t parentId) const {
		return findByParent(parentId) != NullId;
	}
	bool containsWithName(const std::string& name) const {
		return findByName(name) != NullId;
	}
	bool containsIndex(uint32_t index) const {
		return index < nodes.size();
	}

	T& get(uint32_t nodeId) {
		return nodes.at(findById(nodeId));
	}
	T& getByParent(uint32_t parentId) {
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
	const T& get(uint32_t id) const {
		return nodes.at(findById(id));
	}
	const T& getByParent(uint32_t parentId) const {
		return nodes.at(findByParent(parentId));
	}
	const T& getByName(const std::string& name) const {
		return nodes.at(findByName(name));
	}
	const T& getByIndex(uint32_t index) const {
		return nodes.at(index);
	}

	uint32_t getCount() const {
		return nodes.size();
	}
	uint32_t getMaxId() const {
		uint32_t maxId = 0;
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
		for(uint32_t i = 0; i < nodes.size(); i++) {
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

		for(uint32_t i = 0; i < nodes.size(); i++) {
			uint32_t nodeId = nodes[i].id;
			if(nodeId >= indexMap.size()) {
				indexMap.resize(nodeId + 1, NullId);
			}

			indexMap[nodeId] = i;
		}
	}

	std::vector<T> nodes;
	std::vector<uint32_t> indexMap;
};
}