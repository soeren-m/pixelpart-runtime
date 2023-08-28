#pragma once

#include "../common/Curve.h"

namespace pixelpart {
struct Node {
	uint32_t id = nullId;
	uint32_t parentId = nullId;
	std::string name;

	floatd lifetimeStart = 0.0;
	floatd lifetimeDuration = 1.0;
	bool repeat = true;

	Curve<vec3d> position = Curve<vec3d>(0.0, vec3d(0.0));
};
}