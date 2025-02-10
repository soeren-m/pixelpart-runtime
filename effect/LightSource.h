#pragma once

#include "Node.h"
#include "LightSourceType.h"
#include "AnimatedProperty.h"
#include "../common/Types.h"
#include "../common/Math.h"
#include "../common/Id.h"
#include "../json/json.hpp"

namespace pixelpart {
class LightSource : public Node {
public:
	LightSource() = default;
	LightSource(id_t ownId, id_t parentId = id_t());

	virtual void applyInputs(const ComputeGraph::InputSet& inputs) override;

	virtual LightSourceType lightSourceType() const = 0;

	AnimatedProperty<float_t>& attenuation();
	const AnimatedProperty<float_t>& attenuation() const;

	AnimatedProperty<float4_t>& color();
	const AnimatedProperty<float4_t>& color() const;

	AnimatedProperty<float_t>& intensity();
	const AnimatedProperty<float_t>& intensity() const;

private:
	AnimatedProperty<float_t> lightAttenuation = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float4_t> lightColor = AnimatedProperty<float4_t>(float4_t(1.0));
	AnimatedProperty<float_t> lightIntensity = AnimatedProperty<float_t>(1.0);
};

void to_json(nlohmann::ordered_json& j, const LightSource& lightSource);
void from_json(const nlohmann::ordered_json& j, LightSource& lightSource);
}