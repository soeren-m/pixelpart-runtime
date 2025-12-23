#include "Effect.h"
#include "../common/Id.h"
#include "../common/SortedJson.h"
#include <memory>

namespace pixelpart {
Effect::Effect(bool is3d) : effect3d(is3d) {

}

void Effect::enable3d(bool mode) {
	effect3d = mode;
}
bool Effect::is3d() const {
	return effect3d;
}

SceneGraph& Effect::sceneGraph() {
	return effectSceneGraph;
}
const SceneGraph& Effect::sceneGraph() const {
	return effectSceneGraph;
}

ParticleTypeCollection& Effect::particleTypes() {
	return effectParticleTypes;
}
const ParticleTypeCollection& Effect::particleTypes() const {
	return effectParticleTypes;
}

EffectInputCollection& Effect::inputs() {
	return effectInputs;
}
const EffectInputCollection& Effect::inputs() const {
	return effectInputs;
}

TriggerCollection& Effect::triggers() {
	return effectTriggers;
}
const TriggerCollection& Effect::triggers() const {
	return effectTriggers;
}

EventCollection& Effect::events() {
	return effectEvents;
}
const EventCollection& Effect::events() const {
	return effectEvents;
}

ResourceCollection& Effect::resources() {
	return effectResources;
}
const ResourceCollection& Effect::resources() const {
	return effectResources;
}

std::vector<ParticleEmissionPair> Effect::particleEmissionPairs(id_t particleEmitterId) const {
	if(!effectSceneGraph.contains(particleEmitterId)) {
		return std::vector<ParticleEmissionPair>();
	}

	const ParticleEmitter& particleEmitter = effectSceneGraph.at<ParticleEmitter>(particleEmitterId);

	std::vector<ParticleEmissionPair> emissionPairs;
	for(id_t particleTypeId : particleEmitter.particleTypes()) {
		emissionPairs.emplace_back(particleEmitter.id(), particleTypeId);
	}

	return emissionPairs;
}
std::vector<ParticleEmissionPair> Effect::particleEmissionPairs() const {
	std::vector<ParticleEmissionPair> emissionPairs;
	for(const ParticleEmitter* particleEmitter : effectSceneGraph.nodesWithType<ParticleEmitter>()) {
		for(id_t particleTypeId : particleEmitter->particleTypes()) {
			emissionPairs.emplace_back(particleEmitter->id(), particleTypeId);
		}
	}

	return emissionPairs;
}

void Effect::applyInputs() {
	ComputeGraph::InputSet inputValues;
	for(const auto& entry : effectInputs) {
		inputValues[entry.first] = entry.second.value();
	}

	for(std::unique_ptr<Node>& node : effectSceneGraph) {
		node->applyInputs(inputValues);
	}

	for(ParticleType& particleType : effectParticleTypes) {
		particleType.applyInputs(inputValues);
	}
}

void to_json(nlohmann::ordered_json& j, const Effect& effect) {
	j = nlohmann::ordered_json{
		{ "3d", effect.is3d() },
		{ "scene", effect.sceneGraph() },
		{ "particles", effect.particleTypes() },
		{ "inputs", toSortedJson(effect.inputs()) },
		{ "triggers", toSortedJson(effect.triggers()) },
		{ "events", toSortedJson(effect.events()) },
		{ "resources", effect.resources() }
	};
}
void from_json(const nlohmann::ordered_json& j, Effect& effect) {
	effect = Effect(j.value("3d", false));
	effect.sceneGraph() = j.value("scene", SceneGraph());
	effect.particleTypes() = j.value("particles", ParticleTypeCollection());
	effect.inputs() = j.value("inputs", EffectInputCollection());
	effect.triggers() = j.value("triggers", TriggerCollection());
	effect.events() = j.value("events", EventCollection());
	effect.resources() = j.value("resources", ResourceCollection());
}
}
