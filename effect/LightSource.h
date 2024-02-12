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

	AnimatedProperty<vec3_t> direction = AnimatedProperty<vec3_t>(vec3_t(0.0));
	AnimatedProperty<float_t> range = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> attenuation = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> spotAngle = AnimatedProperty<float_t>(45.0);
	AnimatedProperty<float_t> spotAngleAttenuation = AnimatedProperty<float_t>(1.0);

	AnimatedProperty<vec4_t> color = AnimatedProperty<vec4_t>(vec4_t(1.0));
	AnimatedProperty<float_t> intensity = AnimatedProperty<float_t>(1.0);
};

NLOHMANN_JSON_SERIALIZE_ENUM(LightSource::Type, {
	{ LightSource::Type::directional, "directional" },
	{ LightSource::Type::point, "point" },
	{ LightSource::Type::spot, "spot" }
})

void to_json(nlohmann::ordered_json& j, const LightSource& lightSource);
void from_json(const nlohmann::ordered_json& j, LightSource& lightSource);
}