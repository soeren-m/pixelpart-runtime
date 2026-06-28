#include "VortexField.h"

namespace pixelpart {
VortexField::VortexField(id_t ownId, id_t parentId) : ForceField(ownId, parentId) {

}

ForceFieldType VortexField::forceFieldType() const {
	return ForceFieldType::vortex;
}

Node* VortexField::cloneImpl() const {
	return new VortexField(*this);
}

void to_json(nlohmann::ordered_json& j, const VortexField& field) {
	to_json(j, static_cast<const ForceField&>(field));

	j.update(nlohmann::ordered_json{
		{ "force_field_type", ForceFieldType::vortex }
	});
}
void from_json(const nlohmann::ordered_json& j, VortexField& field) {
	field = VortexField(j.at("id").get<id_t>());
	from_json(j, static_cast<ForceField&>(field));
}
}
