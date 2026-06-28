#include "AttractionField.h"

namespace pixelpart {
AttractionField::AttractionField(id_t ownId, id_t parentId) : ForceField(ownId, parentId) {

}

ForceFieldType AttractionField::forceFieldType() const {
	return ForceFieldType::attraction;
}

AnimatedProperty<float_t>& AttractionField::falloffPower() {
	return fieldFalloffPower;
}
const AnimatedProperty<float_t>& AttractionField::falloffPower() const {
	return fieldFalloffPower;
}

Node* AttractionField::cloneImpl() const {
	return new AttractionField(*this);
}

void to_json(nlohmann::ordered_json& j, const AttractionField& field) {
	to_json(j, static_cast<const ForceField&>(field));

	j.update(nlohmann::ordered_json{
		{ "force_field_type", ForceFieldType::attraction },
		{ "attraction", nlohmann::ordered_json{
			{ "falloff_power", field.falloffPower() }
		} }
	});
}
void from_json(const nlohmann::ordered_json& j, AttractionField& field) {
	field = AttractionField(j.at("id").get<id_t>());
	from_json(j, static_cast<ForceField&>(field));

	nlohmann::ordered_json jAttractionField = j.value("attraction", nlohmann::ordered_json::object());
	field.falloffPower() = jAttractionField.value("falloff_power", AnimatedProperty<float_t>(1.0));
}
}
