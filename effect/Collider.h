#pragma once

#include "Node.h"

namespace pixelpart {
struct Collider : public Node {
	std::vector<uint32_t> exclusionList;
	std::vector<vec3d> points = std::vector<vec3d>{
		vec3d(-0.5, 0.0, 0.0),
		vec3d(+0.5, 0.0, 0.0)
	};
	floatd width = 1.0;
	floatd orientation = 0.0;
	bool killOnContact = false;

	Curve<floatd> bounce = Curve<floatd>(0.5);
	Curve<floatd> friction = Curve<floatd>(0.5);
};

void to_json(nlohmann::ordered_json& j, const Collider& collider);
void from_json(const nlohmann::ordered_json& j, Collider& collider);
}