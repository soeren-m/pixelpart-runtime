#include "SceneGraph.h"
#include "NodeType.h"
#include "GroupNode.h"
#include "ParticleEmitter.h"
#include "AttractionField.h"
#include "AccelerationField.h"
#include "VectorField.h"
#include "NoiseField.h"
#include "DragField.h"
#include "VortexField.h"
#include "LineCollider.h"
#include "PlaneCollider.h"
#include "DirectionalLightSource.h"
#include "PointLightSource.h"
#include "SpotLightSource.h"
#include <utility>

namespace pixelpart {
SceneGraph::SceneGraph(const SceneGraph& other) {
	for(const auto& node : other.sceneNodes) {
		sceneNodes.emplace_back(std::move(node->clone()));
	}

	rebuildIndex();
}
SceneGraph::SceneGraph(SceneGraph&& other) :
	sceneNodes(std::move(other.sceneNodes)),
	sceneIndexMap(std::move(other.sceneIndexMap)) {

}
SceneGraph::SceneGraph(std::vector<std::unique_ptr<Node>>&& nodeList) : sceneNodes(std::move(nodeList)) {
	for(auto& node : sceneNodes) {
		if(node->nodeId) {
			continue;
		}

		id_t id = 0;
		while(contains(id)) {
			id++;
		}

		node->nodeId = id;
	}

	rebuildIndex();
}

SceneGraph& SceneGraph::operator=(const SceneGraph& other) {
	sceneNodes.clear();
	for(const auto& node : other.sceneNodes) {
		sceneNodes.emplace_back(std::move(node->clone()));
	}

	rebuildIndex();

	return *this;
}
SceneGraph& SceneGraph::operator=(SceneGraph&& other) {
	sceneNodes = std::move(other.sceneNodes);
	sceneIndexMap = std::move(other.sceneIndexMap);

	return *this;
}

SceneGraph::iterator SceneGraph::begin() {
	return iterator(sceneNodes.begin());
}
SceneGraph::iterator SceneGraph::end() {
	return iterator(sceneNodes.end());
}

SceneGraph::const_iterator SceneGraph::begin() const {
	return const_iterator(sceneNodes.begin());
}
SceneGraph::const_iterator SceneGraph::end() const {
	return const_iterator(sceneNodes.end());
}

Node& SceneGraph::add(const Node& node, id_t baseId) {
	id_t id = node.nodeId
		? id_t(baseId.value() + node.nodeId.value())
		: baseId;
	id_t parentId = node.nodeParentId
		? id_t(baseId.value() + node.nodeParentId.value())
		: id_t();

	while(contains(id)) {
		id++;
	}

	sceneNodes.emplace_back(node.clone());

	auto& insertedNode = sceneNodes.back();
	insertedNode->nodeId = id;
	insertedNode->nodeParentId = parentId;
	rebuildIndex();

	return *insertedNode;
}
Node& SceneGraph::duplicate(id_t id) {
	Node& node = at(id);

	id_t insertedNodeId = 0;
	while(contains(insertedNodeId)) {
		insertedNodeId++;
	}

	sceneNodes.emplace_back(node.clone());

	auto& insertedNode = sceneNodes.back();
	insertedNode->nodeId = insertedNodeId;

	rebuildIndex();

	return *sceneNodes.back();
}
void SceneGraph::remove(id_t id) {
	std::optional<std::size_t> index = indexOf(id);
	if(!index) {
		return;
	}

	const auto& node = sceneNodes[index.value()];
	for(auto& otherNode : sceneNodes) {
		if(otherNode->nodeParentId == node->nodeId) {
			otherNode->parent(node->nodeParentId);
		}
	}

	sceneNodes.erase(sceneNodes.begin() + index.value());
	rebuildIndex();
}
void SceneGraph::clear() {
	sceneNodes.clear();
	rebuildIndex();
}

bool SceneGraph::contains(id_t id) const {
	return indexOf(id).has_value();
}

Node& SceneGraph::at(id_t id) {
	return *sceneNodes.at(indexOf(id).value());
}
const Node& SceneGraph::at(id_t id) const {
	return *sceneNodes.at(indexOf(id).value());
}

void SceneGraph::parent(id_t id, id_t parentId) {
	if(parentId == id) {
		return;
	}

	Node& node = at(id);

	if(isDescendantOf(id, parentId)) {
		for(const auto& otherNode : sceneNodes) {
			if(otherNode->nodeParentId == id) {
				otherNode->parent(node.nodeParentId);
			}
		}
	}

	node.parent(parentId);
}
id_t SceneGraph::parentId(id_t id) const {
	return at(id).nodeParentId;
}
std::vector<id_t> SceneGraph::childIds(id_t id) const {
	std::vector<id_t> result;
	for(const auto& otherNode : sceneNodes) {
		if(otherNode->nodeParentId == id) {
			result.push_back(otherNode->nodeId);
		}
	}

	return result;
}
std::vector<id_t> SceneGraph::siblingIds(id_t id) const {
	std::vector<id_t> result;
	std::optional<std::size_t> index = indexOf(id);
	if(!index) {
		return result;
	}

	const auto& node = sceneNodes[index.value()];

	for(const auto& otherNode : sceneNodes) {
		if(otherNode->nodeId != id && otherNode->nodeParentId == node->nodeParentId) {
			result.push_back(otherNode->nodeId);
		}
	}

	return result;
}
std::vector<id_t> SceneGraph::rootIds() const {
	std::vector<id_t> result;
	for(const auto& node : sceneNodes) {
		if(!node->nodeParentId) {
			result.push_back(node->nodeId);
		}
	}

	return result;
}

Transform SceneGraph::localTransform(id_t id, const EffectRuntimeContext& runtimeContext, bool useTriggers) const {
	return at(id).transform(runtimeContext, useTriggers);
}
Transform SceneGraph::localBaseTransform(id_t id, const EffectRuntimeContext& runtimeContext) const {
	return at(id).baseTransform(runtimeContext);
}
Transform SceneGraph::parentTransform(id_t id, const EffectRuntimeContext& runtimeContext, bool useTriggers) const {
	const Node& node = at(id);
	if(node.nodeParentId) {
		return globalTransform(node.nodeParentId, runtimeContext, useTriggers);
	}

	return Transform();
}
Transform SceneGraph::parentBaseTransform(id_t id, const EffectRuntimeContext& runtimeContext) const {
	const Node& node = at(id);
	if(node.nodeParentId) {
		return globalBaseTransform(node.nodeParentId, runtimeContext);
	}

	return Transform();
}
Transform SceneGraph::globalTransform(id_t id, const EffectRuntimeContext& runtimeContext, bool useTriggers) const {
	const Node& node = at(id);
	if(node.nodeParentId) {
		return globalTransform(node.nodeParentId, runtimeContext, useTriggers) * localTransform(id, runtimeContext, useTriggers);
	}

	return localTransform(id, runtimeContext, useTriggers);
}
Transform SceneGraph::globalBaseTransform(id_t id, const EffectRuntimeContext& runtimeContext) const {
	const Node& node = at(id);
	if(node.nodeParentId) {
		return globalBaseTransform(node.nodeParentId, runtimeContext) * localBaseTransform(id, runtimeContext);
	}

	return localBaseTransform(id, runtimeContext);
}

std::size_t SceneGraph::count() const {
	return sceneNodes.size();
}

void SceneGraph::rebuildIndex() {
	sceneIndexMap.clear();
	sceneIndexMap.reserve(sceneNodes.size());

	for(std::size_t nodeIndex = 0; nodeIndex < sceneNodes.size(); nodeIndex++) {
		id_t id = sceneNodes[nodeIndex]->nodeId;
		if(id.value() >= sceneIndexMap.size()) {
			sceneIndexMap.resize(id.value() + 1, id_t::nullValue);
		}

		sceneIndexMap[id.value()] = nodeIndex;
	}
}
std::optional<std::size_t> SceneGraph::indexOf(id_t id) const {
	if(id.value() < sceneIndexMap.size() && sceneIndexMap[id.value()] != id_t::nullValue) {
		return sceneIndexMap[id.value()];
	}

	return std::nullopt;
}

bool SceneGraph::isDescendantOf(id_t parentId, id_t targetId) const {
	if(!parentId) {
		return false;
	}

	for(id_t childId : childIds(parentId)) {
		if(childId == targetId) {
			return true;
		}

		if(isDescendantOf(childId, targetId)) {
			return true;
		}
	}

	return false;
}

void to_json(nlohmann::ordered_json& j, const SceneGraph& sceneGraph) {
	j = nlohmann::ordered_json::array();

	for(const Node& node : sceneGraph) {
		const GroupNode* groupNode = dynamic_cast<const GroupNode*>(&node);
		const ParticleEmitter* particleEmitter = dynamic_cast<const ParticleEmitter*>(&node);
		const AttractionField* attractionField = dynamic_cast<const AttractionField*>(&node);
		const AccelerationField* accelerationField = dynamic_cast<const AccelerationField*>(&node);
		const VectorField* vectorField = dynamic_cast<const VectorField*>(&node);
		const NoiseField* noiseField = dynamic_cast<const NoiseField*>(&node);
		const DragField* dragField = dynamic_cast<const DragField*>(&node);
		const VortexField* vortexField = dynamic_cast<const VortexField*>(&node);
		const LineCollider* lineCollider = dynamic_cast<const LineCollider*>(&node);
		const PlaneCollider* planeCollider = dynamic_cast<const PlaneCollider*>(&node);
		const DirectionalLightSource* directionLightSource = dynamic_cast<const DirectionalLightSource*>(&node);
		const PointLightSource* pointLightSource = dynamic_cast<const PointLightSource*>(&node);
		const SpotLightSource* spotLightSource = dynamic_cast<const SpotLightSource*>(&node);

		if(groupNode) {
			j.emplace_back(*groupNode);
		}
		else if(particleEmitter) {
			j.emplace_back(*particleEmitter);
		}
		else if(attractionField) {
			j.emplace_back(*attractionField);
		}
		else if(accelerationField) {
			j.emplace_back(*accelerationField);
		}
		else if(vectorField) {
			j.emplace_back(*vectorField);
		}
		else if(noiseField) {
			j.emplace_back(*noiseField);
		}
		else if(dragField) {
			j.emplace_back(*dragField);
		}
		else if(vortexField) {
			j.emplace_back(*vortexField);
		}
		else if(lineCollider) {
			j.emplace_back(*lineCollider);
		}
		else if(planeCollider) {
			j.emplace_back(*planeCollider);
		}
		else if(directionLightSource) {
			j.emplace_back(*directionLightSource);
		}
		else if(pointLightSource) {
			j.emplace_back(*pointLightSource);
		}
		else if(spotLightSource) {
			j.emplace_back(*spotLightSource);
		}
	}
}
void from_json(const nlohmann::ordered_json& j, SceneGraph& sceneGraph) {
	std::vector<std::unique_ptr<Node>> nodes;

	for(const nlohmann::ordered_json& jNode : j) {
		NodeType nodeType = jNode.at("node_type");

		switch(nodeType) {
			case NodeType::group: {
				nodes.emplace_back(std::make_unique<GroupNode>(jNode.get<GroupNode>()));
				break;
			}

			case NodeType::particle_emitter: {
				nodes.emplace_back(std::make_unique<ParticleEmitter>(jNode.get<ParticleEmitter>()));
				break;
			}

			case NodeType::force_field: {
				ForceFieldType forceFieldType = jNode.at("force_field_type");
				switch(forceFieldType) {
					case ForceFieldType::attraction:
						nodes.emplace_back(std::make_unique<AttractionField>(jNode.get<AttractionField>()));
						break;
					case ForceFieldType::acceleration:
						nodes.emplace_back(std::make_unique<AccelerationField>(jNode.get<AccelerationField>()));
						break;
					case ForceFieldType::vector:
						nodes.emplace_back(std::make_unique<VectorField>(jNode.get<VectorField>()));
						break;
					case ForceFieldType::noise:
						nodes.emplace_back(std::make_unique<NoiseField>(jNode.get<NoiseField>()));
						break;
					case ForceFieldType::drag:
						nodes.emplace_back(std::make_unique<DragField>(jNode.get<DragField>()));
						break;
					case ForceFieldType::vortex:
						nodes.emplace_back(std::make_unique<VortexField>(jNode.get<VortexField>()));
						break;
					default:
						break;
				}

				break;
			}

			case NodeType::collider: {
				ColliderType colliderType = jNode.at("collider_type");
				switch(colliderType) {
					case ColliderType::line:
						nodes.emplace_back(std::make_unique<LineCollider>(jNode.get<LineCollider>()));
						break;
					case ColliderType::plane:
						nodes.emplace_back(std::make_unique<PlaneCollider>(jNode.get<PlaneCollider>()));
						break;
					default:
						break;
				}

				break;
			}

			case NodeType::light_source: {
				LightSourceType lightSourceType = jNode.at("light_source_type");
				switch(lightSourceType) {
					case LightSourceType::directional:
						nodes.emplace_back(std::make_unique<DirectionalLightSource>(jNode.get<DirectionalLightSource>()));
						break;
					case LightSourceType::point:
						nodes.emplace_back(std::make_unique<PointLightSource>(jNode.get<PointLightSource>()));
						break;
					case LightSourceType::spot:
						nodes.emplace_back(std::make_unique<SpotLightSource>(jNode.get<SpotLightSource>()));
						break;
					default:
						break;
				}

				break;
			}

			default: {
				break;
			}
		}
	}

	sceneGraph = SceneGraph(std::move(nodes));
}
}
