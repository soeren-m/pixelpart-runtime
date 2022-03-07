#pragma once

#include "Node.h"
#include "Curve.h"
#include <bitset>

namespace pixelpart {
struct Collider : public Node {
	std::bitset<256> emitterMask = std::bitset<256>().set();
	std::vector<vec2d> points = std::vector<vec2d>{ vec2d(-0.5, 0.0), vec2d(+0.5, 0.0) };
	Curve<floatd> bounce = Curve<floatd>(0.5);
	Curve<floatd> friction = Curve<floatd>(0.5);
};

void to_json(nlohmann::ordered_json& j, const Collider& collider);
void from_json(const nlohmann::ordered_json& j, Collider& collider);
}