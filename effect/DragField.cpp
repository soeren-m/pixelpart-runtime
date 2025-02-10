#include "DragField.h"

namespace pixelpart {
DragField::DragField(id_t ownId, id_t parentId) : ForceField(ownId, parentId) {

}

void DragField::applyInputs(const ComputeGraph::InputSet& inputs) {
	ForceField::applyInputs(inputs);

	fieldVelocityInfluence.input(inputs);
	fieldSizeInfluence.input(inputs);
}

ForceFieldType DragField::forceFieldType() const {
	return ForceFieldType::drag;
}

StaticProperty<float_t>& DragField::velocityInfluence() {
	return fieldVelocityInfluence;
}
const StaticProperty<float_t>& DragField::velocityInfluence() const {
	return fieldVelocityInfluence;
}

StaticProperty<float_t>& DragField::sizeInfluence() {
	return fieldSizeInfluence;
}
const StaticProperty<float_t>& DragField::sizeInfluence() const {
	return fieldSizeInfluence;
}

Node* DragField::cloneImpl() const {
	return new DragField(*this);
}

void to_json(nlohmann::ordered_json& j, const DragField& field) {
	to_json(j, static_cast<const ForceField&>(field));

	j.update(nlohmann::ordered_json{
		{ "force_field_type", ForceFieldType::vector },
		{ "drag", nlohmann::ordered_json{
			{ "velocity_influence", field.velocityInfluence() },
			{ "size_influence", field.sizeInfluence() }
		} }
	});
}
void from_json(const nlohmann::ordered_json& j, DragField& field) {
	field = DragField(j.at("id").get<id_t>());
	from_json(j, static_cast<ForceField&>(field));

	nlohmann::ordered_json jDragField = j.value("drag", nlohmann::ordered_json::object());
	field.velocityInfluence() = jDragField.value("velocity_influence", StaticProperty<float_t>(1.0));
	field.sizeInfluence() = jDragField.value("size_influence", StaticProperty<float_t>(1.0));
}
}