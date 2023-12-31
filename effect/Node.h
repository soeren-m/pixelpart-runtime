#pragma once

#include "../property/StaticProperty.h"
#include "../property/AnimatedProperty.h"

namespace pixelpart {
struct Node {
	uint32_t id = nullId;
	uint32_t parentId = nullId;
	std::string name;

	floatd lifetimeStart = 0.0;
	floatd lifetimeDuration = 1.0;
	bool repeat = true;

	AnimatedProperty<vec3d> position = AnimatedProperty<vec3d>(0.0, vec3d(0.0));
};
}