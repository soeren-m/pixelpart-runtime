#include "AttractionField.h"

namespace pixelpart {
AttractionField::AttractionField(id_t ownId, id_t parentId) : ForceField(ownId, parentId) {

}

ForceFieldType AttractionField::forceFieldType() const {
	return ForceFieldType::attraction;
}

Node* AttractionField::cloneImpl() const {
	return new AttractionField(*this);
}

void to_json(nlohmann::ordered_json& j, const AttractionField& field) {
	to_json(j, static_cast<const ForceField&>(field));

	j.update(nlohmann::ordered_json{
		{ "force_field_type", ForceFieldType::attraction }
	});
}
void from_json(const nlohmann::ordered_json& j, AttractionField& field) {
	field = AttractionField(j.at("id").get<id_t>());
	from_json(j, static_cast<ForceField&>(field));
}
}