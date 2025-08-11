#include "Effect.h"
#include "../common/Id.h"
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

ResourceCollection& Effect::resources() {
	return effectResources;
}
const ResourceCollection& Effect::resources() const {
	return effectResources;
}

std::vector<ParticleRuntimeId> Effect::particleRuntimeIds(id_t particleEmitterId) const {
	if(!effectSceneGraph.contains(particleEmitterId)) {
		return std::vector<ParticleRuntimeId>();
	}

	const ParticleEmitter& particleEmitter = effectSceneGraph.at<ParticleEmitter>(particleEmitterId);

	std::vector<ParticleRuntimeId> runtimeIds;
	for(id_t particleTypeId : particleEmitter.particleTypes()) {
		runtimeIds.emplace_back(particleEmitter.id(), particleTypeId);
	}

	return runtimeIds;
}
std::vector<ParticleRuntimeId> Effect::particleRuntimeIds() const {
	std::vector<ParticleRuntimeId> runtimeIds;
	for(const ParticleEmitter* particleEmitter : effectSceneGraph.nodesWithType<ParticleEmitter>()) {
		for(id_t particleTypeId : particleEmitter->particleTypes()) {
			runtimeIds.emplace_back(particleEmitter->id(), particleTypeId);
		}
	}

	return runtimeIds;
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
		{ "inputs", effect.inputs() },
		{ "triggers", effect.triggers() },
		{ "resources", effect.resources() }
	};
}
void from_json(const nlohmann::ordered_json& j, Effect& effect) {
	effect = Effect(j.value("3d", false));
	effect.sceneGraph() = j.value("scene", SceneGraph());
	effect.particleTypes() = j.value("particles", ParticleTypeCollection());
	effect.inputs() = j.value("inputs", EffectInputCollection());
	effect.triggers() = j.value("triggers", TriggerCollection());
	effect.resources() = j.value("resources", ResourceCollection());
}
}
