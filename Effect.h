#pragma once

#include "NodeCollection.h"
#include "ParticleEmitter.h"
#include "Sprite.h"
#include "ForceField.h"
#include "Collider.h"

namespace pixelpart {
struct Effect {
	NodeCollection<ParticleEmitter> particleEmitters;
	NodeCollection<Sprite> sprites;
	NodeCollection<ForceField> forceFields;
	NodeCollection<Collider> colliders;
};

template <typename T>
uint32_t findUnusedNodeId(const std::vector<T>& nodes) {
	uint32_t id = 0;
	bool isUsed = true;

	while(isUsed) {
		id++;
		isUsed = false;

		for(const T& node : nodes) {
			if(node.id == id) {
				isUsed = true;
				break;
			}
		}
	}

	return id;
}

bool isNameUsed(const Effect& effect, const std::string& name);
bool isResourceUsed(const Effect& effect, const std::string& resourceId);

void to_json(nlohmann::ordered_json& j, const Effect& effect);
void from_json(const nlohmann::ordered_json& j, Effect& effect);
}