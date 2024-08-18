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
struct Effect {
	bool is3d = false;

	NodeCollection<ParticleEmitter> particleEmitters;
	NodeCollection<ParticleType> particleTypes;
	NodeCollection<ForceField> forceFields;
	NodeCollection<Collider> colliders;
	NodeCollection<LightSource> lightSources;

	EffectInputCollection inputs;

	ResourceCollection resources;

	void refreshProperties();
};

void to_json(nlohmann::ordered_json& j, const Effect& effect);
void from_json(const nlohmann::ordered_json& j, Effect& effect);
}