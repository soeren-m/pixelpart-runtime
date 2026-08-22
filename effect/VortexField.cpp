#include "VortexField.h"

namespace pixelpart {
VortexField::VortexField(id_t ownId, id_t parentId) : ForceField(ownId, parentId) {

}

ForceFieldType VortexField::forceFieldType() const {
	return ForceFieldType::vortex;
}

AnimatedProperty<float_t>& VortexField::tangentialStrength() {
	return fieldTangentialStrength;
}
const AnimatedProperty<float_t>& VortexField::tangentialStrength() const {
	return fieldTangentialStrength;
}

AnimatedProperty<float_t>& VortexField::radialStrength() {
	return fieldRadialStrength;
}
const AnimatedProperty<float_t>& VortexField::radialStrength() const {
	return fieldRadialStrength;
}

Node* VortexField::cloneImpl() const {
	return new VortexField(*this);
}

void to_json(nlohmann::ordered_json& j, const VortexField& field) {
	to_json(j, static_cast<const ForceField&>(field));

	j.update(nlohmann::ordered_json{
		{ "force_field_type", ForceFieldType::vortex },
		{ "vortex", nlohmann::ordered_json{
			{ "tangential_strength", field.tangentialStrength() },
			{ "radial_strength", field.radialStrength() }
		} }
	});
}
void from_json(const nlohmann::ordered_json& j, VortexField& field) {
	field = VortexField(j.at("id").get<id_t>());
	from_json(j, static_cast<ForceField&>(field));

	nlohmann::ordered_json jVortexField = j.value("vortex", nlohmann::ordered_json::object());
	field.tangentialStrength() = jVortexField.value("tangential_strength", AnimatedProperty<float_t>(1.0));
	field.radialStrength() = jVortexField.value("radial_strength", AnimatedProperty<float_t>(1.0));
}
}
