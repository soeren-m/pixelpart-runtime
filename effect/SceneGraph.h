#pragma once

#include "Node.h"
#include "NodeTransform.h"
#include "../common/Types.h"
#include "../common/Id.h"
#include <memory>
#include <string>
#include <vector>

namespace pixelpart {
class SceneGraph {
public:
	using iterator = typename std::vector<std::unique_ptr<Node>>::iterator;
	using const_iterator = typename std::vector<std::unique_ptr<Node>>::const_iterator;

	SceneGraph() = default;
	SceneGraph(const SceneGraph& other);

	SceneGraph& operator=(const SceneGraph& other);

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

	template <typename T>
	T& add(id_t parentId = id_t()) {
		id_t nodeId = 0u;
		while(contains(nodeId)) {
			nodeId++;
		}

		std::unique_ptr<Node>& node = sceneNodes.emplace_back(new T(nodeId, parentId));
		rebuildIndex();

		return *static_cast<T*>(node.get());
	}

	Node& add(const Node& node, id_t baseId = id_t(0u));
	Node& duplicate(id_t nodeId);

	void set(std::vector<std::unique_ptr<Node>>&& nodeList);

	void remove(id_t nodeId);
	void removeIndex(uint32_t index);
	void clear();

	void parent(id_t nodeId, id_t parentId);
	id_t parentId(id_t nodeId) const;
	std::vector<id_t> childIds(id_t nodeId) const;

	uint32_t indexOf(id_t nodeId) const;
	uint32_t indexOfName(const std::string& name) const;

	bool contains(id_t nodeId) const;
	bool containsIndex(uint32_t index) const;

	template <typename T>
	bool contains(id_t nodeId) const {
		return contains(nodeId) && dynamic_cast<const T*>(&at(nodeId)) != nullptr;
	}

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
		return static_cast<const T&>(at(nodeId));
	}
	template <typename T>
	const T& atIndex(uint32_t index) const {
		return static_cast<const T&>(atIndex(index));
	}

	NodeTransform localTransform(id_t nodeId, float_t time) const;
	NodeTransform localBaseTransform(id_t nodeId, float_t time) const;
	NodeTransform parentTransform(id_t nodeId, float_t time) const;
	NodeTransform parentBaseTransform(id_t nodeId, float_t time) const;
	NodeTransform globalTransform(id_t nodeId, float_t time) const;
	NodeTransform globalBaseTransform(id_t nodeId, float_t time) const;

	uint32_t count() const;
	id_t maxId() const;

	const std::vector<std::unique_ptr<Node>>& nodes() const;

	template <typename T>
	std::vector<T*> nodesWithType() {
		std::vector<T*> typedNodes;
		for(std::unique_ptr<Node>& node : sceneNodes) {
			T* typedNode = dynamic_cast<T*>(node.get());
			if(typedNode) {
				typedNodes.push_back(typedNode);
			}
		}

		return typedNodes;
	}
	template <typename T>
	std::vector<const T*> nodesWithType() const {
		std::vector<const T*> typedNodes;
		for(const std::unique_ptr<Node>& node : sceneNodes) {
			const T* typedNode = dynamic_cast<const T*>(node.get());
			if(typedNode) {
				typedNodes.push_back(typedNode);
			}
		}

		return typedNodes;
	}

private:
	void rebuildIndex();

	std::vector<std::unique_ptr<Node>> sceneNodes;
	std::vector<uint32_t> indexMap;
};

void to_json(nlohmann::ordered_json& j, const SceneGraph& sceneGraph);
void from_json(const nlohmann::ordered_json& j, SceneGraph& sceneGraph);
}