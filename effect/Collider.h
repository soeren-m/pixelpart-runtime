#pragma once

#include "Node.h"

namespace pixelpart {
struct Collider : public Node {
	std::vector<uint32_t> exclusionList;
	std::vector<vec3d> points = std::vector<vec3d>{
		vec3d(-0.5, 0.0, 0.0),
		vec3d(+0.5, 0.0, 0.0)
	};
	StaticProperty<floatd> width = StaticProperty<floatd>(1.0);
	StaticProperty<floatd> orientation = StaticProperty<floatd>(0.0);
	StaticProperty<bool> killOnContact = StaticProperty<bool>(false);

	AnimatedProperty<floatd> bounce = AnimatedProperty<floatd>(0.5);
	AnimatedProperty<floatd> friction = AnimatedProperty<floatd>(0.5);
};

void to_json(nlohmann::ordered_json& j, const Collider& collider);
void from_json(const nlohmann::ordered_json& j, Collider& collider);
}