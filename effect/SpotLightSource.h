#pragma once

#include "LightSource.h"

namespace pixelpart {
class SpotLightSource : public LightSource {
public:
	SpotLightSource() = default;
	SpotLightSource(id_t ownId, id_t parentId = id_t());

	virtual void applyInputs(const ComputeGraph::InputSet& inputs) override;

	virtual LightSourceType lightSourceType() const override;

	AnimatedProperty<float_t>& spotAngle();
	const AnimatedProperty<float_t>& spotAngle() const;

	AnimatedProperty<float_t>& spotAngleAttenuation();
	const AnimatedProperty<float_t>& spotAngleAttenuation() const;

protected:
	virtual Node* cloneImpl() const override;

private:
	AnimatedProperty<float_t> lightSpotAngle = AnimatedProperty<float_t>(45.0);
	AnimatedProperty<float_t> lightSpotAngleAttenuation = AnimatedProperty<float_t>(1.0);
};

void to_json(nlohmann::ordered_json& j, const SpotLightSource& lightSource);
void from_json(const nlohmann::ordered_json& j, SpotLightSource& lightSource);
}