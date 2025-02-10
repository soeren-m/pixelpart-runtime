#pragma once

#include "ForceField.h"
#include "../json/json.hpp"

namespace pixelpart {
class AttractionField : public ForceField {
public:
	AttractionField() = default;
	AttractionField(id_t ownId, id_t parentId = id_t());

	virtual ForceFieldType forceFieldType() const override;

protected:
	virtual Node* cloneImpl() const override;
};

void to_json(nlohmann::ordered_json& j, const AttractionField& field);
void from_json(const nlohmann::ordered_json& j, AttractionField& field);
}