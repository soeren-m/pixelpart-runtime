#pragma once

#include "ForceField.h"
#include "../json/json.hpp"

namespace pixelpart {
class VortexField : public ForceField {
public:
	VortexField() = default;
	VortexField(id_t ownId, id_t parentId = id_t());

	virtual ForceFieldType forceFieldType() const override;

	AnimatedProperty<float_t>& tangentialStrength();
	const AnimatedProperty<float_t>& tangentialStrength() const;

	AnimatedProperty<float_t>& radialStrength();
	const AnimatedProperty<float_t>& radialStrength() const;

protected:
	virtual Node* cloneImpl() const override;

private:
	AnimatedProperty<float_t> fieldTangentialStrength = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> fieldRadialStrength = AnimatedProperty<float_t>(1.0);
};

void to_json(nlohmann::ordered_json& j, const VortexField& field);
void from_json(const nlohmann::ordered_json& j, VortexField& field);
}
