#pragma once

#include "SceneGraph.h"
#include "ParticleTypeCollection.h"
#include "EffectInputCollection.h"
#include "ResourceCollection.h"
#include "../json/json.hpp"

namespace pixelpart {
class Effect {
public:
	Effect() = default;
	Effect(bool is3d);

	void enable3d(bool mode);
	bool is3d() const;

	SceneGraph& sceneGraph();
	const SceneGraph& sceneGraph() const;

	ParticleTypeCollection& particleTypes();
	const ParticleTypeCollection& particleTypes() const;

	EffectInputCollection& inputs();
	const EffectInputCollection& inputs() const;

	ResourceCollection& resources();
	const ResourceCollection& resources() const;

	void refreshProperties();

private:
	bool effect3d = false;

	SceneGraph effectSceneGraph;
	ParticleTypeCollection effectParticleTypes;

	EffectInputCollection effectInputs;

	ResourceCollection effectResources;
};

void to_json(nlohmann::ordered_json& j, const Effect& effect);
void from_json(const nlohmann::ordered_json& j, Effect& effect);
}