#pragma once

#include "Node.h"

namespace pixelpart {
struct LightSource : public Node {
	enum class Type {
		directional = 0,
		point = 1,
		spot = 2
	};

	Type type = Type::directional;

	AnimatedProperty<vec3d> direction = AnimatedProperty<vec3d>(vec3d(0.0));
	AnimatedProperty<floatd> range = AnimatedProperty<floatd>(1.0);
	AnimatedProperty<floatd> attenuation = AnimatedProperty<floatd>(1.0);
	AnimatedProperty<floatd> spotAngle = AnimatedProperty<floatd>(45.0);
	AnimatedProperty<floatd> spotAngleAttenuation = AnimatedProperty<floatd>(1.0);

	AnimatedProperty<vec4d> color = AnimatedProperty<vec4d>(vec4d(1.0));
	AnimatedProperty<floatd> intensity = AnimatedProperty<floatd>(1.0);
};

NLOHMANN_JSON_SERIALIZE_ENUM(LightSource::Type, {
	{ LightSource::Type::directional, "directional" },
	{ LightSource::Type::point, "point" },
	{ LightSource::Type::spot, "spot" }
})

void to_json(nlohmann::ordered_json& j, const LightSource& lightSource);
void from_json(const nlohmann::ordered_json& j, LightSource& lightSource);
}