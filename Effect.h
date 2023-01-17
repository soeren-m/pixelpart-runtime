#pragma once

#include "NodeCollection.h"
#include "ParticleEmitter.h"
#include "ParticleType.h"
#include "ForceField.h"
#include "Collider.h"

namespace pixelpart {
struct Effect {
	NodeCollection<ParticleEmitter> particleEmitters;
	NodeCollection<ParticleType> particleTypes;
	NodeCollection<ForceField> forceFields;
	NodeCollection<Collider> colliders;
	bool is3d = false;
};

bool isNameUsed(const Effect& effect, const std::string& name);
bool isResourceUsed(const Effect& effect, const std::string& resourceId);

void to_json(nlohmann::ordered_json& j, const Effect& effect);
void from_json(const nlohmann::ordered_json& j, Effect& effect);
}