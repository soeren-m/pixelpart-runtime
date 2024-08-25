#pragma once

#include "NodeCollection.h"
#include "ParticleEmitter.h"
#include "ParticleType.h"
#include "ForceField.h"
#include "Collider.h"
#include "LightSource.h"
#include "EffectInputCollection.h"
#include "ResourceCollection.h"
#include "../json/json.hpp"

namespace pixelpart {
class Effect {
public:
	Effect() = default;
	Effect(bool effect3d);

	NodeCollection<ParticleEmitter>& particleEmitters();
	const NodeCollection<ParticleEmitter>& particleEmitters() const;

	// TODO

	EffectInputCollection& inputs();
	const EffectInputCollection& inputs() const;

	ResourceCollection& resources();
	const ResourceCollection& resources() const;

	void refreshProperties();

private:
	bool is3d = false;

	NodeCollection<ParticleEmitter> particleEmitters;
	NodeCollection<ParticleType> particleTypes;
	NodeCollection<ForceField> forceFields;
	NodeCollection<Collider> colliders;
	NodeCollection<LightSource> lightSources;

	EffectInputCollection inputs;
	ResourceCollection resources;
};

void to_json(nlohmann::ordered_json& j, const Effect& effect);
void from_json(const nlohmann::ordered_json& j, Effect& effect);
}