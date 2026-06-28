#pragma once

#include "ForceField.h"
#include "AnimatedProperty.h"
#include "../types/Types.h"
#include "../json/json.hpp"

namespace pixelpart {
class AttractionField : public ForceField {
public:
	AttractionField() = default;
	AttractionField(id_t ownId, id_t parentId = id_t());

	virtual ForceFieldType forceFieldType() const override;

	AnimatedProperty<float_t>& falloffPower();
	const AnimatedProperty<float_t>& falloffPower() const;

protected:
	virtual Node* cloneImpl() const override;

private:
	AnimatedProperty<float_t> fieldFalloffPower = AnimatedProperty<float_t>(1.0);
};

void to_json(nlohmann::ordered_json& j, const AttractionField& field);
void from_json(const nlohmann::ordered_json& j, AttractionField& field);
}
