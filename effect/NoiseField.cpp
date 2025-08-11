#include "NoiseField.h"

namespace pixelpart {
NoiseField::NoiseField(id_t ownId, id_t parentId) : ForceField(ownId, parentId) {

}

void NoiseField::applyInputs(const ComputeGraph::InputSet& inputs) {
	ForceField::applyInputs(inputs);

	fieldNoiseOctaves.input(inputs);
	fieldNoiseFrequency.input(inputs);
	fieldNoisePersistence.input(inputs);
	fieldNoiseLacunarity.input(inputs);
	fieldNoiseAnimationTimeScale.input(inputs);
	fieldNoiseAnimationTimeBase.input(inputs);
}

ForceFieldType NoiseField::forceFieldType() const {
	return ForceFieldType::noise;
}

StaticProperty<int_t>& NoiseField::noiseOctaves() {
	return fieldNoiseOctaves;
}
const StaticProperty<int_t>& NoiseField::noiseOctaves() const {
	return fieldNoiseOctaves;
}

AnimatedProperty<float_t>& NoiseField::noiseFrequency() {
	return fieldNoiseFrequency;
}
const AnimatedProperty<float_t>& NoiseField::noiseFrequency() const {
	return fieldNoiseFrequency;
}

AnimatedProperty<float_t>& NoiseField::noisePersistence() {
	return fieldNoisePersistence;
}
const AnimatedProperty<float_t>& NoiseField::noisePersistence() const {
	return fieldNoisePersistence;
}

AnimatedProperty<float_t>& NoiseField::noiseLacunarity() {
	return fieldNoiseLacunarity;
}
const AnimatedProperty<float_t>& NoiseField::noiseLacunarity() const {
	return fieldNoiseLacunarity;
}

void NoiseField::noiseAnimated(bool enable) {
	fieldNoiseAnimated = enable;
}
bool NoiseField::noiseAnimated() const {
	return fieldNoiseAnimated;
}

StaticProperty<float_t>& NoiseField::noiseAnimationTimeScale() {
	return fieldNoiseAnimationTimeScale;
}
const StaticProperty<float_t>& NoiseField::noiseAnimationTimeScale() const {
	return fieldNoiseAnimationTimeScale;
}

StaticProperty<float_t>& NoiseField::noiseAnimationTimeBase() {
	return fieldNoiseAnimationTimeBase;
}
const StaticProperty<float_t>& NoiseField::noiseAnimationTimeBase() const {
	return fieldNoiseAnimationTimeBase;
}

Node* NoiseField::cloneImpl() const {
	return new NoiseField(*this);
}

void to_json(nlohmann::ordered_json& j, const NoiseField& field) {
	to_json(j, static_cast<const ForceField&>(field));
	j.update(nlohmann::ordered_json{
		{ "force_field_type", ForceFieldType::noise },
		{ "noise", nlohmann::ordered_json{
			{ "octaves", field.noiseOctaves() },
			{ "frequency", field.noiseFrequency() },
			{ "persistence", field.noisePersistence() },
			{ "lacunarity", field.noiseLacunarity() },
			{ "animated", field.noiseAnimated() },
			{ "animation_time_scale", field.noiseAnimationTimeScale() },
			{ "animation_time_base", field.noiseAnimationTimeBase() }
		} }
	});
}
void from_json(const nlohmann::ordered_json& j, NoiseField& field) {
	field = NoiseField(j.at("id").get<id_t>());
	from_json(j, static_cast<ForceField&>(field));

	nlohmann::ordered_json jNoiseField = j.value("noise", nlohmann::ordered_json());
	field.noiseOctaves() = jNoiseField.value("octaves", StaticProperty<int_t>(4));
	field.noiseFrequency() = jNoiseField.value("frequency", AnimatedProperty<float_t>(1.0));
	field.noisePersistence() = jNoiseField.value("persistence", AnimatedProperty<float_t>(0.5));
	field.noiseLacunarity() = jNoiseField.value("lacunarity", AnimatedProperty<float_t>(2.0));
	field.noiseAnimated(jNoiseField.value("animated", false));
	field.noiseAnimationTimeScale() = jNoiseField.value("animation_time_scale", StaticProperty<float_t>(1.0));
	field.noiseAnimationTimeBase() = jNoiseField.value("animation_time_base", StaticProperty<float_t>(0.0));
}
}
