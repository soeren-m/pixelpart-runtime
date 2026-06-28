#pragma once

#include "ForceField.h"
#include "../json/json.hpp"

namespace pixelpart {
class VortexField : public ForceField {
public:
	VortexField() = default;
	VortexField(id_t ownId, id_t parentId = id_t());

	virtual ForceFieldType forceFieldType() const override;

protected:
	virtual Node* cloneImpl() const override;
};

void to_json(nlohmann::ordered_json& j, const VortexField& field);
void from_json(const nlohmann::ordered_json& j, VortexField& field);
}
