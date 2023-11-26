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

	Curve<vec3d> direction = Curve<vec3d>(vec3d(0.0));
	Curve<floatd> range = Curve<floatd>(1.0);
	Curve<floatd> attenuation = Curve<floatd>(1.0);
	Curve<floatd> spotAngle = Curve<floatd>(45.0);
	Curve<floatd> spotAngleAttenuation = Curve<floatd>(1.0);

	Curve<vec4d> color = Curve<vec4d>(vec4d(1.0));
	Curve<floatd> intensity = Curve<floatd>(1.0);
};

NLOHMANN_JSON_SERIALIZE_ENUM(LightSource::Type, {
	{ LightSource::Type::directional, "directional" },
	{ LightSource::Type::point, "point" },
	{ LightSource::Type::spot, "spot" }
})

void to_json(nlohmann::ordered_json& j, const LightSource& lightSource);
void from_json(const nlohmann::ordered_json& j, LightSource& lightSource);
}