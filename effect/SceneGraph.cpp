#include "SceneGraph.h"
#include "NodeType.h"
#include "GroupNode.h"
#include "ParticleEmitter.h"
#include "AttractionField.h"
#include "AccelerationField.h"
#include "VectorField.h"
#include "NoiseField.h"
#include "DragField.h"
#include "LineCollider.h"
#include "PlaneCollider.h"
#include "DirectionalLightSource.h"
#include "PointLightSource.h"
#include "SpotLightSource.h"
#include <algorithm>

namespace pixelpart {
SceneGraph::SceneGraph(const SceneGraph& other) {
	for(const std::unique_ptr<Node>& node : other.sceneNodes) {
		sceneNodes.emplace_back(std::move(node->clone()));
	}

	rebuildIndex();
}

SceneGraph& SceneGraph::operator=(const SceneGraph& other) {
	sceneNodes.clear();
	for(const std::unique_ptr<Node>& node : other.sceneNodes) {
		sceneNodes.emplace_back(std::move(node->clone()));
	}

	rebuildIndex();

	return *this;
}

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

Node& SceneGraph::add(const Node& node, id_t baseId) {
	id_t nodeId = node.nodeId
		? id_t(baseId.value() + node.nodeId.value())
		: baseId;
	id_t parentId = node.nodeParentId
		? id_t(baseId.value() + node.nodeParentId.value())
		: id_t();

	while(contains(nodeId)) {
		nodeId++;
	}

	sceneNodes.emplace_back(node.clone());

	std::unique_ptr<Node>& insertedNode = sceneNodes.back();
	insertedNode->nodeId = nodeId;
	insertedNode->nodeParentId = parentId;
	rebuildIndex();

	return *insertedNode;
}
Node& SceneGraph::duplicate(id_t nodeId) {
	Node& node = at(nodeId);

	id_t insertedNodeId = 0;
	while(contains(insertedNodeId)) {
		insertedNodeId++;
	}

	sceneNodes.emplace_back(node.clone());

	std::unique_ptr<Node>& insertedNode = sceneNodes.back();
	insertedNode->nodeId = insertedNodeId;

	rebuildIndex();

	return *sceneNodes.back();
}

void SceneGraph::set(std::vector<std::unique_ptr<Node>>&& nodeList) {
	sceneNodes = std::move(nodeList);

	for(std::unique_ptr<Node>& node : sceneNodes) {
		if(node->id()) {
			continue;
		}

		id_t nodeId = 0;
		while(contains(nodeId)) {
			nodeId++;
		}

		node->nodeId = nodeId;
	}

	rebuildIndex();
}

void SceneGraph::remove(id_t nodeId) {
	removeIndex(indexOf(nodeId));
}
void SceneGraph::removeIndex(std::uint32_t index) {
	if(index >= sceneNodes.size()) {
		return;
	}

	const std::unique_ptr<Node>& node = sceneNodes[index];
	for(std::unique_ptr<Node>& otherNode : sceneNodes) {
		if(otherNode->parentId() == node->id()) {
			otherNode->parent(node->parentId());
		}
	}

	sceneNodes.erase(sceneNodes.begin() + index);
	rebuildIndex();
}
void SceneGraph::clear() {
	sceneNodes.clear();
	rebuildIndex();
}

void SceneGraph::parent(id_t nodeId, id_t parentId) {
	Node& node = at(nodeId);
	for(std::unique_ptr<Node>& otherNode : sceneNodes) {
		if(otherNode->parentId() == nodeId) {
			otherNode->parent(node.parentId());
		}
	}

	node.parent(parentId);
}
id_t SceneGraph::parentId(id_t nodeId) const {
	return at(nodeId).parentId();
}
std::vector<id_t> SceneGraph::childIds(id_t nodeId) const {
	std::vector<id_t> result;
	for(const std::unique_ptr<Node>& otherNode : sceneNodes) {
		if(otherNode->parentId() == nodeId) {
			result.push_back(otherNode->id());
		}
	}

	return result;
}

std::uint32_t SceneGraph::indexOf(id_t nodeId) const {
	return nodeId.value() < indexMap.size()
		? indexMap[nodeId.value()]
		: id_t::nullValue;
}
std::uint32_t SceneGraph::indexOfName(const std::string& name) const {
	for(std::uint32_t index = 0; index < sceneNodes.size(); index++) {
		if(sceneNodes[index]->nodeName == name) {
			return index;
		}
	}

	return id_t::nullValue;
}

bool SceneGraph::contains(id_t nodeId) const {
	return indexOf(nodeId) != id_t::nullValue;
}
bool SceneGraph::containsIndex(std::uint32_t index) const {
	return index < sceneNodes.size();
}

Node& SceneGraph::at(id_t nodeId) {
	return *sceneNodes.at(indexOf(nodeId));
}
Node& SceneGraph::atIndex(std::uint32_t index) {
	return *sceneNodes.at(index);
}
const Node& SceneGraph::at(id_t nodeId) const {
	return *sceneNodes.at(indexOf(nodeId));
}
const Node& SceneGraph::atIndex(std::uint32_t index) const {
	return *sceneNodes.at(index);
}

Transform SceneGraph::localTransform(id_t nodeId, const RuntimeContext& runtimeContext, bool useTriggers) const {
	return at(nodeId).transform(runtimeContext, useTriggers);
}
Transform SceneGraph::localBaseTransform(id_t nodeId, const RuntimeContext& runtimeContext) const {
	return at(nodeId).baseTransform(runtimeContext);
}
Transform SceneGraph::parentTransform(id_t nodeId, const RuntimeContext& runtimeContext, bool useTriggers) const {
	const Node& node = at(nodeId);
	if(node.parentId()) {
		return globalTransform(node.parentId(), runtimeContext, useTriggers);
	}

	return Transform();
}
Transform SceneGraph::parentBaseTransform(id_t nodeId, const RuntimeContext& runtimeContext) const {
	const Node& node = at(nodeId);
	if(node.parentId()) {
		return globalBaseTransform(node.parentId(), runtimeContext);
	}

	return Transform();
}
Transform SceneGraph::globalTransform(id_t nodeId, const RuntimeContext& runtimeContext, bool useTriggers) const {
	const Node& node = at(nodeId);
	if(node.parentId()) {
		return globalTransform(node.parentId(), runtimeContext, useTriggers) * localTransform(nodeId, runtimeContext, useTriggers);
	}

	return localTransform(nodeId, runtimeContext, useTriggers);
}
Transform SceneGraph::globalBaseTransform(id_t nodeId, const RuntimeContext& runtimeContext) const {
	const Node& node = at(nodeId);
	if(node.parentId()) {
		return globalBaseTransform(node.parentId(), runtimeContext) * localBaseTransform(nodeId, runtimeContext);
	}

	return localBaseTransform(nodeId, runtimeContext);
}

std::uint32_t SceneGraph::count() const {
	return static_cast<std::uint32_t>(sceneNodes.size());
}
id_t SceneGraph::maxId() const {
	id_t maxId = 0;
	for(const std::unique_ptr<Node>& node : sceneNodes) {
		maxId = std::max(maxId, node->nodeId);
	}

	return maxId;
}

const std::vector<std::unique_ptr<Node>>& SceneGraph::nodes() const {
	return sceneNodes;
}

void SceneGraph::rebuildIndex() {
	indexMap.clear();
	indexMap.reserve(sceneNodes.size());

	for(std::uint32_t nodeIndex = 0; nodeIndex < sceneNodes.size(); nodeIndex++) {
		id_t nodeId = sceneNodes[nodeIndex]->nodeId;
		if(nodeId.value() >= indexMap.size()) {
			indexMap.resize(nodeId.value() + 1, id_t::nullValue);
		}

		indexMap[nodeId.value()] = nodeIndex;
	}
}

void to_json(nlohmann::ordered_json& j, const SceneGraph& sceneGraph) {
	j = nlohmann::ordered_json::array();

	for(const std::unique_ptr<Node>& node : sceneGraph.nodes()) {
		const GroupNode* groupNode = dynamic_cast<const GroupNode*>(node.get());
		const ParticleEmitter* particleEmitter = dynamic_cast<const ParticleEmitter*>(node.get());
		const AttractionField* attractionField = dynamic_cast<const AttractionField*>(node.get());
		const AccelerationField* accelerationField = dynamic_cast<const AccelerationField*>(node.get());
		const VectorField* vectorField = dynamic_cast<const VectorField*>(node.get());
		const NoiseField* noiseField = dynamic_cast<const NoiseField*>(node.get());
		const DragField* dragField = dynamic_cast<const DragField*>(node.get());
		const LineCollider* lineCollider = dynamic_cast<const LineCollider*>(node.get());
		const PlaneCollider* planeCollider = dynamic_cast<const PlaneCollider*>(node.get());
		const DirectionalLightSource* directionLightSource = dynamic_cast<const DirectionalLightSource*>(node.get());
		const PointLightSource* pointLightSource = dynamic_cast<const PointLightSource*>(node.get());
		const SpotLightSource* spotLightSource = dynamic_cast<const SpotLightSource*>(node.get());

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
				nodes.emplace_back(new GroupNode(jNode.get<GroupNode>()));
				break;
			}

			case NodeType::particle_emitter: {
				nodes.emplace_back(new ParticleEmitter(jNode.get<ParticleEmitter>()));
				break;
			}

			case NodeType::force_field: {
				ForceFieldType forceFieldType = jNode.at("force_field_type");
				switch(forceFieldType) {
					case ForceFieldType::attraction:
						nodes.emplace_back(new AttractionField(jNode.get<AttractionField>()));
						break;
					case ForceFieldType::acceleration:
						nodes.emplace_back(new AccelerationField(jNode.get<AccelerationField>()));
						break;
					case ForceFieldType::vector:
						nodes.emplace_back(new VectorField(jNode.get<VectorField>()));
						break;
					case ForceFieldType::noise:
						nodes.emplace_back(new NoiseField(jNode.get<NoiseField>()));
						break;
					case ForceFieldType::drag:
						nodes.emplace_back(new DragField(jNode.get<DragField>()));
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
						nodes.emplace_back(new LineCollider(jNode.get<LineCollider>()));
						break;
					case ColliderType::plane:
						nodes.emplace_back(new PlaneCollider(jNode.get<PlaneCollider>()));
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
						nodes.emplace_back(new DirectionalLightSource(jNode.get<DirectionalLightSource>()));
						break;
					case LightSourceType::point:
						nodes.emplace_back(new PointLightSource(jNode.get<PointLightSource>()));
						break;
					case LightSourceType::spot:
						nodes.emplace_back(new SpotLightSource(jNode.get<SpotLightSource>()));
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

	sceneGraph.set(std::move(nodes));
}
}
