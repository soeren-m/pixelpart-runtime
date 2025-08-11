#include "AccelerationField.h"

namespace pixelpart {
AccelerationField::AccelerationField(id_t ownId, id_t parentId) : ForceField(ownId, parentId) {

}

void AccelerationField::applyInputs(const ComputeGraph::InputSet& inputs) {
	ForceField::applyInputs(inputs);

	fieldAccelerationDirection.input(inputs);
	fieldAccelerationDirectionVariance.input(inputs);
	fieldAccelerationStrengthVariance.input(inputs);
}

ForceFieldType AccelerationField::forceFieldType() const {
	return ForceFieldType::acceleration;
}

AnimatedProperty<float3_t>& AccelerationField::accelerationDirection() {
	return fieldAccelerationDirection;
}
const AnimatedProperty<float3_t>& AccelerationField::accelerationDirection() const {
	return fieldAccelerationDirection;
}

AnimatedProperty<float_t>& AccelerationField::accelerationDirectionVariance() {
	return fieldAccelerationDirectionVariance;
}
const AnimatedProperty<float_t>& AccelerationField::accelerationDirectionVariance() const {
	return fieldAccelerationDirectionVariance;
}

AnimatedProperty<float_t>& AccelerationField::accelerationStrengthVariance() {
	return fieldAccelerationStrengthVariance;
}
const AnimatedProperty<float_t>& AccelerationField::accelerationStrengthVariance() const {
	return fieldAccelerationStrengthVariance;
}

void AccelerationField::accelerationGrid(std::int32_t x, std::int32_t y, std::int32_t z) {
	fieldAccelerationGridSize[0] = x;
	fieldAccelerationGridSize[1] = y;
	fieldAccelerationGridSize[2] = z;
	fieldAccelerationDirectionGrid.clear();
	fieldAccelerationStrengthGrid.clear();

	std::size_t cellCount = static_cast<std::size_t>(std::max(x * y * z, 0));
	fieldAccelerationDirectionGrid.resize(cellCount, float3_t(0.0));
	fieldAccelerationStrengthGrid.resize(cellCount, 0.0);
}
void AccelerationField::accelerationGrid(std::int32_t x, std::int32_t y, std::int32_t z,
	const std::vector<float3_t>& directionGrid, const std::vector<float_t>& strengthGrid) {
	fieldAccelerationGridSize[0] = x;
	fieldAccelerationGridSize[1] = y;
	fieldAccelerationGridSize[2] = z;
	fieldAccelerationDirectionGrid = directionGrid;
	fieldAccelerationStrengthGrid = strengthGrid;

	std::size_t cellCount = static_cast<std::size_t>(std::max(x * y * z, 0));
	fieldAccelerationDirectionGrid.resize(cellCount, float3_t(0.0));
	fieldAccelerationStrengthGrid.resize(cellCount, 0.0);
}
const std::vector<float3_t>& AccelerationField::accelerationDirectionGrid() const {
	return fieldAccelerationDirectionGrid;
}
const std::vector<float_t>& AccelerationField::accelerationStrengthGrid() const {
	return fieldAccelerationStrengthGrid;
}
std::int32_t AccelerationField::accelerationGridSizeX() const {
	return fieldAccelerationGridSize[0];
}
std::int32_t AccelerationField::accelerationGridSizeY() const {
	return fieldAccelerationGridSize[1];
}
std::int32_t AccelerationField::accelerationGridSizeZ() const {
	return fieldAccelerationGridSize[2];
}

Node* AccelerationField::cloneImpl() const {
	return new AccelerationField(*this);
}

void to_json(nlohmann::ordered_json& j, const AccelerationField& field) {
	to_json(j, static_cast<const ForceField&>(field));

	j.update(nlohmann::ordered_json{
		{ "force_field_type", ForceFieldType::acceleration },
		{ "acceleration", nlohmann::ordered_json{
			{ "direction", field.accelerationDirection() },
			{ "direction_variance", field.accelerationDirectionVariance() },
			{ "strength_variance", field.accelerationStrengthVariance() },
			{ "grid_size_x", field.accelerationGridSizeX() },
			{ "grid_size_y", field.accelerationGridSizeY() },
			{ "grid_size_z", field.accelerationGridSizeZ() },
			{ "grid_direction", field.accelerationDirectionGrid() },
			{ "grid_strength", field.accelerationStrengthGrid() }
		} }
	});
}
void from_json(const nlohmann::ordered_json& j, AccelerationField& field) {
	field = AccelerationField(j.at("id").get<id_t>());
	from_json(j, static_cast<ForceField&>(field));

	nlohmann::ordered_json jAccelerationField = j.value("acceleration", nlohmann::ordered_json::object());
	field.accelerationDirection() = jAccelerationField.value("direction", AnimatedProperty<float3_t>(float3_t(0.0)));
	field.accelerationDirectionVariance() = jAccelerationField.value("direction_variance", AnimatedProperty<float_t>(0.0));
	field.accelerationStrengthVariance() = jAccelerationField.value("strength_variance", AnimatedProperty<float_t>(0.0));
	field.accelerationGrid(
		jAccelerationField.value("grid_size_x", 1),
		jAccelerationField.value("grid_size_y", 1),
		jAccelerationField.value("grid_size_z", 1),
		jAccelerationField.value("grid_direction", std::vector<float3_t>{ float3_t(0.0) }),
		jAccelerationField.value("grid_strength", std::vector<float_t>{ 0.0 }));
}
}
