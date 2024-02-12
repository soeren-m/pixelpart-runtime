#pragma once

#include "Node.h"

namespace pixelpart {
struct Collider : public Node {
	std::vector<id_t> exclusionList;
	std::vector<vec3_t> points = std::vector<vec3_t>{
		vec3_t(-0.5, 0.0, 0.0),
		vec3_t(+0.5, 0.0, 0.0)
	};
	StaticProperty<float_t> width = StaticProperty<float_t>(1.0);
	StaticProperty<float_t> orientation = StaticProperty<float_t>(0.0);
	StaticProperty<bool> killOnContact = StaticProperty<bool>(false);

	AnimatedProperty<float_t> bounce = AnimatedProperty<float_t>(0.5);
	AnimatedProperty<float_t> friction = AnimatedProperty<float_t>(0.5);
};

void to_json(nlohmann::ordered_json& j, const Collider& collider);
void from_json(const nlohmann::ordered_json& j, Collider& collider);
}