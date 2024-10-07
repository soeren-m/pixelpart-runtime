#pragma once

#include "Node.h"
#include "../common/Types.h"
#include "../common/Math.h"
#include "../common/Id.h"
#include "AnimatedProperty.h"
#include "../json/json.hpp"

namespace pixelpart {
class LightSource : public Node {
public:
	enum class Type {
		directional = 0,
		point = 1,
		spot = 2
	};

	LightSource() = default;
	LightSource(id_t ownId, id_t parentId = id_t());

	void type(Type type);
	Type type() const;

	AnimatedProperty<float3_t>& direction();
	const AnimatedProperty<float3_t>& direction() const;

	AnimatedProperty<float_t>& range();
	const AnimatedProperty<float_t>& range() const;

	AnimatedProperty<float_t>& attenuation();
	const AnimatedProperty<float_t>& attenuation() const;

	AnimatedProperty<float_t>& spotAngle();
	const AnimatedProperty<float_t>& spotAngle() const;

	AnimatedProperty<float_t>& spotAngleAttenuation();
	const AnimatedProperty<float_t>& spotAngleAttenuation() const;

	AnimatedProperty<float4_t>& color();
	const AnimatedProperty<float4_t>& color() const;

	AnimatedProperty<float_t>& intensity();
	const AnimatedProperty<float_t>& intensity() const;

private:
	Type lightType = Type::directional;

	AnimatedProperty<float3_t> lightDirection = AnimatedProperty<float3_t>(float3_t(0.0));
	AnimatedProperty<float_t> lightRange = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> lightAttenuation = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> lightSpotAngle = AnimatedProperty<float_t>(45.0);
	AnimatedProperty<float_t> lightSpotAngleAttenuation = AnimatedProperty<float_t>(1.0);

	AnimatedProperty<float4_t> lightColor = AnimatedProperty<float4_t>(float4_t(1.0));
	AnimatedProperty<float_t> lightIntensity = AnimatedProperty<float_t>(1.0);
};

NLOHMANN_JSON_SERIALIZE_ENUM(LightSource::Type, {
	{ LightSource::Type::directional, "directional" },
	{ LightSource::Type::point, "point" },
	{ LightSource::Type::spot, "spot" }
})

void to_json(nlohmann::ordered_json& j, const LightSource& lightSource);
void from_json(const nlohmann::ordered_json& j, LightSource& lightSource);
}