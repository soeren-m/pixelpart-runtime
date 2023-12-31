#pragma once

#include "NodeCollection.h"
#include "ParticleEmitter.h"
#include "ParticleType.h"
#include "ForceField.h"
#include "Collider.h"
#include "LightSource.h"
#include "EffectResourceSet.h"
#include "EffectInputSet.h"

namespace pixelpart {
struct Effect {
	bool is3d = false;

	NodeCollection<ParticleEmitter> particleEmitters;
	NodeCollection<ParticleType> particleTypes;
	NodeCollection<ForceField> forceFields;
	NodeCollection<Collider> colliders;
	NodeCollection<LightSource> lightSources;

	EffectResourceSet resources;

	EffectInputSet inputs;
};

template <typename T>
void refreshEffectProperty(const Effect& effect, StaticProperty<T>& property) {
	ComputeGraph::InputSet inputValues;
	for(const auto& entry : effect.inputs) {
		inputValues[entry.first] = entry.second.value;
	}

	property.refresh(inputValues);
}

template <typename T>
void refreshEffectProperty(const Effect& effect, AnimatedProperty<T>& property) {
	ComputeGraph::InputSet inputValues;
	for(const auto& entry : effect.inputs) {
		inputValues[entry.first] = entry.second.value;
	}

	property.refresh(inputValues);
}

void refreshEffectProperties(Effect& effect);

bool isNameUsedInEffect(const Effect& effect, const std::string& name);
bool isResourceUsedInEffect(const Effect& effect, const std::string& resourceId);

void to_json(nlohmann::ordered_json& j, const Effect& effect);
void from_json(const nlohmann::ordered_json& j, Effect& effect);
}