#pragma once

#include "../property/StaticProperty.h"
#include "../property/AnimatedProperty.h"

namespace pixelpart {
struct Node {
	id_t id = nullId;
	id_t parentId = nullId;
	std::string name;

	float_t lifetimeStart = 0.0;
	float_t lifetimeDuration = 1.0;
	bool repeat = true;

	AnimatedProperty<vec3_t> position = AnimatedProperty<vec3_t>(0.0, vec3_t(0.0));
};
}