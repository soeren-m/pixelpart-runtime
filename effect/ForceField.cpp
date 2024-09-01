#include "ForceField.h"
#include <algorithm>

namespace pixelpart {
ForceField::ForceField(id_t ownId, id_t parentId) : Node(ownId, parentId) {

}

void ForceField::type(Type type) {
	fieldType = type;
}
ForceField::Type ForceField::type() const {
	return fieldType;
}

NodeExclusionSet& ForceField::exclusionSet() {
	return fieldExclusionSet;
}
const NodeExclusionSet& ForceField::exclusionSet() const {
	return fieldExclusionSet;
}

AnimatedProperty<vec3_t>& ForceField::size() {
	return fieldSize;
}
const AnimatedProperty<vec3_t>& ForceField::size() const {
	return fieldSize;
}

AnimatedProperty<vec3_t>& ForceField::orientation() {
	return fieldOrientation;
}
const AnimatedProperty<vec3_t>& ForceField::orientation() const {
	return fieldOrientation;
}

AnimatedProperty<float_t>& ForceField::strength() {
	return fieldStrength;
}
const AnimatedProperty<float_t>& ForceField::strength() const {
	return fieldStrength;
}

AnimatedProperty<vec3_t>& ForceField::accelerationDirection() {
	return fieldAccelerationDirection;
}
const AnimatedProperty<vec3_t>& ForceField::accelerationDirection() const {
	return fieldAccelerationDirection;
}

AnimatedProperty<float_t>& ForceField::accelerationDirectionVariance() {
	return fieldAccelerationDirectionVariance;
}
const AnimatedProperty<float_t>& ForceField::accelerationDirectionVariance() const {
	return fieldAccelerationDirectionVariance;
}

AnimatedProperty<float_t>& ForceField::accelerationStrengthVariance() {
	return fieldAccelerationStrengthVariance;
}
const AnimatedProperty<float_t>& ForceField::accelerationStrengthVariance() const {
	return fieldAccelerationStrengthVariance;
}

void ForceField::accelerationGridSize(int32_t x, int32_t y, int32_t z) {
	fieldAccelerationGridSize[0] = x;
	fieldAccelerationGridSize[1] = y;
	fieldAccelerationGridSize[2] = z;
}
int32_t ForceField::accelerationGridSizeX() const {
	return fieldAccelerationGridSize[0];
}
int32_t ForceField::accelerationGridSizeY() const {
	return fieldAccelerationGridSize[1];
}
int32_t ForceField::accelerationGridSizeZ() const {
	return fieldAccelerationGridSize[2];
}

void ForceField::vectorResourceId(const std::string& resourceId) {
	fieldVectorResourceId = resourceId;
}
const std::string& ForceField::vectorResourceId() const {
	return fieldVectorResourceId;
}

void ForceField::vectorFilter(Filter filter) {
	fieldVectorFilter = filter;
}
ForceField::Filter ForceField::vectorFilter() const {
	return fieldVectorFilter;
}

AnimatedProperty<float_t>& ForceField::vectorTightness() {
	return fieldVectorTightness;
}
const AnimatedProperty<float_t>& ForceField::vectorTightness() const {
	return fieldVectorTightness;
}

// TODO


void to_json(nlohmann::ordered_json& j, const ForceField::NoiseField& field) {
	j = nlohmann::ordered_json{
		{ "octaves", field.octaves },
		{ "frequency", field.frequency },
		{ "persistence", field.persistence },
		{ "lacunarity", field.lacunarity },

		{ "animated", field.animated },
		{ "animation_time_scale", field.animationTimeScale },
		{ "animation_time_base", field.animationTimeBase }
	};
}
void to_json(nlohmann::ordered_json& j, const ForceField::DragField& field) {
	j = nlohmann::ordered_json{
		{ "velocity_influence", field.velocityInfluence },
		{ "size_influence", field.sizeInfluence }
	};
}
void to_json(nlohmann::ordered_json& j, const ForceField& forceField) {
	j = nlohmann::ordered_json{
		{ "id", toJson(forceField.id) },
		{ "parent_id", toJson(forceField.parentId) },
		{ "name", forceField.name() },
		{ "lifetime_start", forceField.start() },
		{ "lifetime_duration", forceField.duration() },
		{ "repeat", forceField.repeat() },
		{ "position", forceField.position() },

		{ "type", forceField.type() },
		{ "exclusion_list", forceField.exclusionSet() },
		{ "size", forceField.size() },
		{ "orientation", forceField.orientation() },
		{ "strength", forceField.strength() },

		{ "attraction_field", nlohmann::ordered_json{ } },
		{ "acceleration_field", nlohmann::ordered_json{
			{ "direction", forceField.accelerationDirection() },
			{ "direction_variance", forceField.accelerationDirectionVariance() },
			{ "strength_variance", forceField.accelerationStrengthVariance() },
			{ "grid_size_x", forceField.accelerationGridSizeX() },
			{ "grid_size_y", forceField.accelerationGridSizeY() },
			{ "grid_size_z", forceField.accelerationGridSizeZ() },
			{ "grid_direction", forceField.directionGrid },
			{ "grid_strength", forceField.strengthGrid }
		} },
		{ "vector_field", nlohmann::ordered_json{
			{ "resource_id", forceField.vectorResourceId() },
			{ "filter", forceField.vectorFilter() },
			{ "tightness", forceField.vectorTightness() }
		} },
		{ "noise_field", forceField.noiseField },
		{ "drag_field", forceField.dragField }
	};
}
void from_json(const nlohmann::ordered_json& j, ForceField::AttractionField& field) {
	field = ForceField::AttractionField();
}
void from_json(const nlohmann::ordered_json& j, ForceField::AccelerationField& field) {
	field = ForceField::AccelerationField();

	field.direction, j, "direction");
	field.directionVariance, j, "direction_variance");
	field.strengthVariance, j, "strength_variance");

	field.gridSize[0], j, "grid_size_x");
	field.gridSize[1], j, "grid_size_y");
	field.gridSize[2], j, "grid_size_z");
	field.directionGrid, j, "grid_direction");
	field.strengthGrid, j, "grid_strength");

	std::size_t numGridCells = static_cast<std::size_t>(
		std::max(field.gridSize[0] * field.gridSize[1] * field.gridSize[2], 0));
	field.directionGrid.resize(numGridCells);
	field.strengthGrid.resize(numGridCells);
}
void from_json(const nlohmann::ordered_json& j, ForceField::VectorField& field) {
	field = ForceField::VectorField();

	field.resourceId, j, "resource_id");
	field.filter, j, "filter");

	field.tightness, j, "tightness");
}
void from_json(const nlohmann::ordered_json& j, ForceField::NoiseField& field) {
	field = ForceField::NoiseField();

	field.octaves, j, "octaves");
	field.frequency, j, "frequency");
	field.persistence, j, "persistence");
	field.lacunarity, j, "lacunarity");

	field.animated, j, "animated");
	field.animationTimeScale, j, "animation_time_scale");
	field.animationTimeBase, j, "animation_time_base");
}
void from_json(const nlohmann::ordered_json& j, ForceField::DragField& field) {
	field = ForceField::DragField();

	field.velocityInfluence, j, "velocity_influence");
	field.sizeInfluence, j, "size_influence");
}
void from_json(const nlohmann::ordered_json& j, ForceField& forceField) {
	forceField = ForceField(
		j.at("id"),
		j.value("parent_id", nullId));

	forceField.name(j.value("name", ""));
	forceField.start(j.value("lifetime_start", 0.0));
	forceField.duration(j.value("lifetime_duration", 1.0));
	forceField.repeat(j.value("repeat", true));
	forceField.position() = j.value("position", AnimatedProperty<vec3_t>(0.0, vec3_t(0.0)));

	forceField.type(j.value("type", ForceField::Type::attraction_field));
	forceField.exclusionList, j, "exclusion_list");
	forceField.size, j, "size");
	forceField.orientation, j, "orientation");

	forceField.strength, j, "strength");

	forceField.attractionField, j, "attraction_field");
	forceField.accelerationField, j, "acceleration_field");
	forceField.vectorField, j, "vector_field");
	forceField.noiseField, j, "noise_field");
	forceField.dragField, j, "drag_field");
}
}