#pragma once

#include "SceneGraph.h"
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
#include "ParticleTypeCollection.h"
#include "EffectInputCollection.h"
#include "TriggerCollection.h"
#include "ResourceCollection.h"
#include "ParticleEmissionPair.h"
#include "../json/json.hpp"
#include <vector>

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

	TriggerCollection& triggers();
	const TriggerCollection& triggers() const;

	ResourceCollection& resources();
	const ResourceCollection& resources() const;

	std::vector<ParticleEmissionPair> particleEmissionPairs(id_t particleEmitterId) const;
	std::vector<ParticleEmissionPair> particleEmissionPairs() const;

	void applyInputs();

private:
	bool effect3d = false;

	SceneGraph effectSceneGraph;
	ParticleTypeCollection effectParticleTypes;
	EffectInputCollection effectInputs;
	TriggerCollection effectTriggers;
	ResourceCollection effectResources;
};

void to_json(nlohmann::ordered_json& j, const Effect& effect);
void from_json(const nlohmann::ordered_json& j, Effect& effect);
}
