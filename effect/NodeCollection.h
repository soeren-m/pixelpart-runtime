#pragma once

#include "../common/Types.h"
#include "../common/Id.h"
#include "Node.h"
#include <string>
#include <vector>
#include <algorithm>

namespace pixelpart {
template <typename T>
class NodeCollection {
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

		for(T& node : nodes) {
			if(node.id()) {
				continue;
			}

			id_t newId = 0u;
			bool used = true;

			while(used) {
				newId++;
				used = false;

				for(const T& node : nodes) {
					if(node.id() == newId) {
						used = true;
						break;
					}
				}
			}

			node.nodeId = newId;
		}

		updateIndexMap();
	}

	id_t set(const T& node, id_t baseId) {
		id_t nodeId = node.nodeId
			? id_t(baseId.value() + node.nodeId.value())
			: baseId;
		id_t parentId = node.nodeParentId
			? id_t(baseId.value() + node.nodeParentId.value())
			: id_t();

		nodes.push_back(node);

		T& insertedNode = nodes.back();
		insertedNode.nodeId = nodeId;
		insertedNode.nodeParentId = parentId;
		updateIndexMap();

		return nodeId;
	}

	id_t add(const T& node) {
		id_t nodeId = node.nodeId ? node.nodeId : id_t(0u);
		while(contains(nodeId)) {
			nodeId++;
		}

		nodes.push_back(node);
		nodes.back().nodeId = nodeId;
		updateIndexMap();

		return nodeId;
	}

	id_t duplicate(id_t nodeId, const std::string& nameExtension = " (copy)") {
		if(!contains(nodeId)) {
			return id_t();
		}

		T otherNode = at(nodeId);
		otherNode.nodeId = id_t();
		otherNode.nodeParentId = id_t();

		do {
			otherNode.nodeName += nameExtension;
		}
		while(containsName(otherNode.nodeName));

		return add(otherNode);
	}

	void remove(id_t nodeId) {
		removeAt(indexOf(nodeId));
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

	uint32_t indexOf(id_t nodeId) const {
		return nodeId.value() < indexMap.size()
			? indexMap[nodeId.value()]
			: id_t::nullValue;
	}
	uint32_t indexOfParent(id_t parentId) const {
		for(uint32_t i = 0u; i < nodes.size(); i++) {
			if(nodes[i].nodeParentId == parentId) {
				return i;
			}
		}

		return id_t::nullValue;
	}
	uint32_t indexOfName(const std::string& name) const {
		for(uint32_t i = 0u; i < nodes.size(); i++) {
			if(nodes[i].nodeName == name) {
				return i;
			}
		}

		return id_t::nullValue;
	}

	bool contains(id_t nodeId) const {
		return indexOf(nodeId) != id_t::nullValue;
	}
	bool containsParent(id_t parentId) const {
		return indexOfParent(parentId) != id_t::nullValue;
	}
	bool containsName(const std::string& name) const {
		return indexOfName(name) != id_t::nullValue;
	}
	bool containsIndex(uint32_t index) const {
		return index < nodes.size();
	}

	T& at(id_t nodeId) {
		return nodes.at(indexOf(nodeId));
	}
	const T& at(id_t id) const {
		return nodes.at(indexOf(id));
	}

	T& atParent(id_t parentId) {
		return nodes.at(indexOfParent(parentId));
	}
	const T& atParent(id_t parentId) const {
		return nodes.at(indexOfParent(parentId));
	}

	T& atName(const std::string& name) {
		return nodes.at(indexOfName(name));
	}
	const T& atName(const std::string& name) const {
		return nodes.at(indexOfName(name));
	}

	T& atIndex(uint32_t index) {
		return nodes.at(index);
	}
	const T& atIndex(uint32_t index) const {
		return nodes.at(index);
	}

	const std::vector<T>& container() const {
		return nodes;
	}

	uint32_t count() const {
		return static_cast<uint32_t>(nodes.size());
	}
	id_t maxId() const {
		id_t maxId = 0u;
		for(const T& node : nodes) {
			maxId = std::max(maxId, node.nodeId);
		}

		return maxId;
	}

	template <typename TFunc>
	std::vector<T> sorted(TFunc compare) const {
		std::vector<T> sortedNodes = nodes;
		std::sort(sortedNodes.begin(), sortedNodes.end(), [this, &compare](const T& node1, const T& node2) {
			return compare(node1, node2);
		});

		return sortedNodes;
	}

	template <typename TFunc>
	std::vector<uint32_t> sortedIndices(TFunc compare) const {
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
			id_t nodeId = nodes[i].nodeId;
			if(nodeId.value() >= indexMap.size()) {
				indexMap.resize(nodeId.value() + 1u, id_t::nullValue);
			}

			indexMap[nodeId.value()] = i;
		}
	}

	std::vector<T> nodes;
	std::vector<uint32_t> indexMap;
};

template <typename T>
void to_json(nlohmann::ordered_json& j, const NodeCollection<T>& collection) {
	j = collection.container();
}

template <typename T>
void from_json(const nlohmann::ordered_json& j, NodeCollection<T>& collection) {
	std::vector<T> nodes = j;
	collection.set(nodes);
}
}