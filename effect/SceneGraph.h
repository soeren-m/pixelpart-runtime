#pragma once

#include "../common/Types.h"
#include "../common/Id.h"
#include "Node.h"
#include "NodeTransform.h"
#include <string>
#include <memory>
#include <vector>

namespace pixelpart {
class SceneGraph {
public:
	using iterator = typename std::vector<std::unique_ptr<Node>>::iterator;
	using const_iterator = typename std::vector<std::unique_ptr<Node>>::const_iterator;

	SceneGraph() = default;

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

	void set(const std::vector<std::unique_ptr<Node>>& nodeList);
	id_t set(std::unique_ptr<Node> node, id_t baseId);
	id_t add(std::unique_ptr<Node> node);
	id_t duplicate(id_t nodeId, const std::string& nameExtension = " (copy)");

	void remove(id_t nodeId);
	void removeAt(uint32_t index);
	void removeAll();

	uint32_t indexOf(id_t nodeId) const;
	uint32_t indexOfName(const std::string& name) const;

	bool contains(id_t nodeId) const;
	bool containsIndex(uint32_t index) const;

	Node& at(id_t nodeId);
	Node& atIndex(uint32_t index);
	const Node& at(id_t nodeId) const;
	const Node& atIndex(uint32_t index) const;

	template <typename T>
	T& at(id_t nodeId) {
		return static_cast<T&>(at(nodeId));
	}
	template <typename T>
	T& atIndex(uint32_t index) {
		return static_cast<T&>(atIndex(index));
	}
	template <typename T>
	const T& at(id_t nodeId) const {
		return static_cast<T&>(at(nodeId));
	}
	template <typename T>
	const T& atIndex(uint32_t index) const {
		return static_cast<T&>(atIndex(index));
	}

	NodeTransform localTransform(const Node& node, float_t t) const;
	NodeTransform localTransform(id_t nodeId, float_t t) const;

	NodeTransform globalTransform(const Node& node, float_t t) const;
	NodeTransform globalTransform(id_t nodeId, float_t t) const; // TODO

	uint32_t count() const;
	id_t maxId() const;

	const std::vector<std::unique_ptr<Node>>& nodes() const;

private:
	std::vector<std::unique_ptr<Node>> sceneNodes;
	std::vector<uint32_t> indexMap;
};
}