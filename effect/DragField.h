#pragma once

#include "ForceField.h"
#include "StaticProperty.h"
#include "../common/Types.h"
#include "../json/json.hpp"

namespace pixelpart {
class DragField : public ForceField {
public:
	DragField() = default;
	DragField(id_t ownId, id_t parentId = id_t());

	virtual void applyInputs(const ComputeGraph::InputSet& inputs) override;

	virtual ForceFieldType forceFieldType() const override;

	StaticProperty<float_t>& velocityInfluence();
	const StaticProperty<float_t>& velocityInfluence() const;

	StaticProperty<float_t>& sizeInfluence();
	const StaticProperty<float_t>& sizeInfluence() const;

protected:
	virtual Node* cloneImpl() const override;

private:
	StaticProperty<float_t> fieldVelocityInfluence = StaticProperty<float_t>(1.0);
	StaticProperty<float_t> fieldSizeInfluence = StaticProperty<float_t>(1.0);
};

void to_json(nlohmann::ordered_json& j, const DragField& field);
void from_json(const nlohmann::ordered_json& j, DragField& field);
}