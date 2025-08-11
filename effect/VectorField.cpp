#include "VectorField.h"

namespace pixelpart {
VectorField::VectorField(id_t ownId, id_t parentId) : ForceField(ownId, parentId) {

}

void VectorField::applyInputs(const ComputeGraph::InputSet& inputs) {
	ForceField::applyInputs(inputs);

	fieldTightness.input(inputs);
}
bool VectorField::usesResource(const std::string& resourceId) const {
	return resourceId == fieldVectorFieldResourceId;
}

ForceFieldType VectorField::forceFieldType() const {
	return ForceFieldType::vector;
}

void VectorField::vectorFieldResourceId(const std::string& resourceId) {
	fieldVectorFieldResourceId = resourceId;
}
const std::string& VectorField::vectorFieldResourceId() const {
	return fieldVectorFieldResourceId;
}

void VectorField::vectorFieldFilter(Filter filter) {
	fieldVectorFieldFilter = filter;
}
VectorField::Filter VectorField::vectorFieldFilter() const {
	return fieldVectorFieldFilter;
}

AnimatedProperty<float_t>& VectorField::tightness() {
	return fieldTightness;
}
const AnimatedProperty<float_t>& VectorField::tightness() const {
	return fieldTightness;
}

Node* VectorField::cloneImpl() const {
	return new VectorField(*this);
}

void to_json(nlohmann::ordered_json& j, const VectorField& field) {
	to_json(j, static_cast<const ForceField&>(field));
	j.update(nlohmann::ordered_json{
		{ "force_field_type", ForceFieldType::vector },
		{ "vector", nlohmann::ordered_json{
			{ "resource_id", field.vectorFieldResourceId() },
			{ "filter", field.vectorFieldFilter() },
			{ "tightness", field.tightness() }
		} }
	});
}
void from_json(const nlohmann::ordered_json& j, VectorField& field) {
	field = VectorField(j.at("id").get<id_t>());
	from_json(j, static_cast<ForceField&>(field));

	nlohmann::ordered_json jVectorField = j.value("vector", nlohmann::ordered_json());
	field.vectorFieldResourceId(jVectorField.value("resource_id", ""));
	field.vectorFieldFilter(jVectorField.value("filter", VectorField::Filter::none));
	field.tightness() = jVectorField.value("tightness", AnimatedProperty<float_t>(1.0));
}
}
