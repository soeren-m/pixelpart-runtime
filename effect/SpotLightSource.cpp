#include "SpotLightSource.h"

namespace pixelpart {
SpotLightSource::SpotLightSource(id_t ownId, id_t parentId) : LightSource(ownId, parentId) {

}

void SpotLightSource::applyInputs(const ComputeGraph::InputSet& inputs) {
	LightSource::applyInputs(inputs);

	lightSpotAngle.input(inputs);
	lightSpotAngleAttenuation.input(inputs);
}

LightSourceType SpotLightSource::lightSourceType() const {
	return LightSourceType::spot;
}

AnimatedProperty<float_t>& SpotLightSource::spotAngle() {
	return lightSpotAngle;
}
const AnimatedProperty<float_t>& SpotLightSource::spotAngle() const {
	return lightSpotAngle;
}

AnimatedProperty<float_t>& SpotLightSource::spotAngleAttenuation() {
	return lightSpotAngleAttenuation;
}
const AnimatedProperty<float_t>& SpotLightSource::spotAngleAttenuation() const {
	return lightSpotAngleAttenuation;
}

Node* SpotLightSource::cloneImpl() const {
	return new SpotLightSource(*this);
}

void to_json(nlohmann::ordered_json& j, const SpotLightSource& lightSource) {
	to_json(j, static_cast<const LightSource&>(lightSource));

	j.update(nlohmann::ordered_json{
		{ "light_source_type", LightSourceType::spot },
		{ "spot_angle", lightSource.spotAngle() },
		{ "spot_angle_attenuation", lightSource.spotAngleAttenuation() }
	});
}
void from_json(const nlohmann::ordered_json& j, SpotLightSource& lightSource) {
	lightSource = SpotLightSource(j.at("id").get<id_t>());
	from_json(j, static_cast<LightSource&>(lightSource));

	lightSource.spotAngle() = j.value("spot_angle", AnimatedProperty<float_t>(45.0));
	lightSource.spotAngleAttenuation() = j.value("spot_angle_attenuation", AnimatedProperty<float_t>(1.0));
}
}