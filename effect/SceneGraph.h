#pragma once

#include "Node.h"
#include "EffectRuntimeContext.h"
#include "Transform.h"
#include "DereferenceIterator.h"
#include "../types/Id.h"
#include "../json/json.hpp"
#include <memory>
#include <optional>
#include <vector>

namespace pixelpart {
class SceneGraph {
public:
	using iterator = DereferenceIterator<std::vector<std::unique_ptr<Node>>::iterator>;
	using const_iterator = DereferenceIterator<std::vector<std::unique_ptr<Node>>::const_iterator>;

	SceneGraph() = default;
	SceneGraph(const SceneGraph& other);
	SceneGraph(SceneGraph&& other);
	SceneGraph(std::vector<std::unique_ptr<Node>>&& nodeList);

	SceneGraph& operator=(const SceneGraph& other);
	SceneGraph& operator=(SceneGraph&& other);

	iterator begin();
	iterator end();

	const_iterator begin() const;
	const_iterator end() const;

	template <typename T>
	T& add(id_t parentId = id_t()) {
		id_t id = 0;
		while(contains(id)) {
			id++;
		}

		auto& node = sceneNodes.emplace_back(std::make_unique<T>(id, parentId));
		rebuildIndex();

		return *static_cast<T*>(node.get());
	}

	Node& add(const Node& node, id_t baseId = id_t(0));
	Node& duplicate(id_t id);
	void remove(id_t id);
	void clear();

	bool contains(id_t id) const;

	template <typename T>
	bool contains(id_t id) const {
		return contains(id) && dynamic_cast<const T*>(&at(id)) != nullptr;
	}

	Node& at(id_t id);
	const Node& at(id_t id) const;

	template <typename T>
	T& at(id_t id) {
		return static_cast<T&>(at(id));
	}
	template <typename T>
	const T& at(id_t id) const {
		return static_cast<const T&>(at(id));
	}

	void parent(id_t id, id_t parentId);
	id_t parentId(id_t id) const;
	std::vector<id_t> childIds(id_t id) const;
	std::vector<id_t> siblingIds(id_t id) const;
	std::vector<id_t> rootIds() const;

	Transform localTransform(id_t id, const EffectRuntimeContext& runtimeContext, bool useTriggers = true) const;
	Transform localBaseTransform(id_t id, const EffectRuntimeContext& runtimeContext) const;
	Transform parentTransform(id_t id, const EffectRuntimeContext& runtimeContext, bool useTriggers = true) const;
	Transform parentBaseTransform(id_t id, const EffectRuntimeContext& runtimeContext) const;
	Transform globalTransform(id_t id, const EffectRuntimeContext& runtimeContext, bool useTriggers = true) const;
	Transform globalBaseTransform(id_t id, const EffectRuntimeContext& runtimeContext) const;

	template <typename T>
	std::vector<T*> nodesWithType() {
		std::vector<T*> typedNodes;
		for(auto& node : sceneNodes) {
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
		for(const auto& node : sceneNodes) {
			const T* typedNode = dynamic_cast<const T*>(node.get());
			if(typedNode) {
				typedNodes.push_back(typedNode);
			}
		}

		return typedNodes;
	}

	std::size_t count() const;

private:
	void rebuildIndex();
	std::optional<std::size_t> indexOf(id_t id) const;

	bool isDescendantOf(id_t parentId, id_t targetId) const;

	std::vector<std::unique_ptr<Node>> sceneNodes;
	std::vector<std::size_t> sceneIndexMap;
};

void to_json(nlohmann::ordered_json& j, const SceneGraph& sceneGraph);
void from_json(const nlohmann::ordered_json& j, SceneGraph& sceneGraph);
}
