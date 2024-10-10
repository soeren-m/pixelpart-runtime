#pragma once

#include "../common/Types.h"
#include "../common/Id.h"
#include "Node.h"
#include <string>
#include <memory>
#include <vector>
#include <algorithm>

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
	uint32_t indexOfParent(id_t parentId) const;
	uint32_t indexOfName(const std::string& name) const;

	bool contains(id_t nodeId) const;
	bool containsParent(id_t parentId) const;
	bool containsName(const std::string& name) const;
	bool containsIndex(uint32_t index) const;

	Node& at(id_t nodeId);
	Node& atIndex(uint32_t index);
	const Node& at(id_t id) const;
	const Node& atIndex(uint32_t index) const;

	float3_t globalPosition(id_t nodeId, float_t t) const; // TODO

	uint32_t count() const;
	id_t maxId() const;

	const std::vector<std::unique_ptr<Node>>& nodes() const;

private:
	std::vector<std::unique_ptr<Node>> nodes;
	std::vector<uint32_t> indexMap;
};
}