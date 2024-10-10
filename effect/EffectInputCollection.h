#pragma once

#include "../common/Types.h"
#include "../common/Id.h"
#include "EffectInput.h"
#include "StaticProperty.h"
#include "AnimatedProperty.h"
#include <unordered_map>

namespace pixelpart {
using EffectInputCollection = std::unordered_map<id_t, EffectInput>;

template <typename T>
void refreshProperty(const EffectInputCollection& inputs, StaticProperty<T>& property) {
	ComputeGraph::InputSet inputValues;
	for(const auto& entry : inputs) {
		inputValues[entry.first] = entry.second.value;
	}

	property.input(inputValues);
}

template <typename T>
void refreshProperty(const EffectInputCollection& inputs, AnimatedProperty<T>& property) {
	ComputeGraph::InputSet inputValues;
	for(const auto& entry : inputs) {
		inputValues[entry.first] = entry.second.value;
	}

	property.input(inputValues);
}
}