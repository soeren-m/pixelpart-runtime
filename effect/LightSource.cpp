#include "LightSource.h"

namespace pixelpart {
LightSource::LightSource(id_t ownId, id_t parentId) : Node(ownId, parentId) {

}

void LightSource::type(Type type) {
	lightType = type;
}
LightSource::Type LightSource::type() const {
	return lightType;
}

AnimatedProperty<float3_t>& LightSource::direction() {
	return lightDirection;
}
const AnimatedProperty<float3_t>& LightSource::direction() const {
	return lightDirection;
}

AnimatedProperty<float_t>& LightSource::range() {
	return lightRange;
}
const AnimatedProperty<float_t>& LightSource::range() const {
	return lightRange;
}

AnimatedProperty<float_t>& LightSource::attenuation() {
	return lightAttenuation;
}
const AnimatedProperty<float_t>& LightSource::attenuation() const {
	return lightAttenuation;
}

AnimatedProperty<float_t>& LightSource::spotAngle() {
	return lightSpotAngle;
}
const AnimatedProperty<float_t>& LightSource::spotAngle() const {
	return lightSpotAngle;
}

AnimatedProperty<float_t>& LightSource::spotAngleAttenuation() {
	return lightSpotAngleAttenuation;
}
const AnimatedProperty<float_t>& LightSource::spotAngleAttenuation() const {
	return lightSpotAngleAttenuation;
}

AnimatedProperty<float4_t>& LightSource::color() {
	return lightColor;
}
const AnimatedProperty<float4_t>& LightSource::color() const {
	return lightColor;
}

AnimatedProperty<float_t>& LightSource::intensity() {
	return lightIntensity;
}
const AnimatedProperty<float_t>& LightSource::intensity() const {
	return lightIntensity;
}

void to_json(nlohmann::ordered_json& j, const LightSource& lightSource) {
	j = nlohmann::ordered_json{
		{ "id", lightSource.id() },
		{ "parent_id", lightSource.parentId() },
		{ "name", lightSource.name() },
		{ "lifetime_start", lightSource.start() },
		{ "lifetime_duration", lightSource.duration() },
		{ "repeat", lightSource.repeat() },
		{ "position", lightSource.position() },

		{ "type", lightSource.type() },
		{ "direction", lightSource.direction() },
		{ "range", lightSource.range() },
		{ "attenuation", lightSource.attenuation() },
		{ "spot_angle", lightSource.spotAngle() },
		{ "spot_angle_attenuation", lightSource.spotAngleAttenuation() },
		{ "color", lightSource.color() },
		{ "intensity", lightSource.intensity() }
	};
}
void from_json(const nlohmann::ordered_json& j, LightSource& lightSource) {
	lightSource = LightSource(
		j.at("id"),
		j.value("parent_id", id_t()));

	lightSource.name(j.value("name", ""));
	lightSource.start(j.value("lifetime_start", 0.0));
	lightSource.duration(j.value("lifetime_duration", 1.0));
	lightSource.repeat(j.value("repeat", true));
	lightSource.position() = j.value("position", AnimatedProperty<float3_t>(0.0, float3_t(0.0)));

	lightSource.type(j.value("type", LightSource::Type::directional));
	lightSource.direction() = j.value("direction", AnimatedProperty<float3_t>(float3_t(0.0)));
	lightSource.range() = j.value("range", AnimatedProperty<float_t>(1.0));
	lightSource.attenuation() = j.value("attenuation", AnimatedProperty<float_t>(1.0));
	lightSource.spotAngle() = j.value("spot_angle", AnimatedProperty<float_t>(45.0));
	lightSource.spotAngleAttenuation() = j.value("spot_angle_attenuation", AnimatedProperty<float_t>(1.0));
	lightSource.color() = j.value("color", AnimatedProperty<float4_t>(float4_t(1.0)));
	lightSource.intensity() = j.value("intensity", AnimatedProperty<float_t>(1.0));
}
}