#include "ForceField.h"
#include "../common/Json.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const ForceField::AttractionField& field) {
	j = nlohmann::ordered_json{

	};
}
void to_json(nlohmann::ordered_json& j, const ForceField::AccelerationField& field) {
	j = nlohmann::ordered_json{
		{ "direction", field.direction },
		{ "direction_variance", field.directionVariance },
		{ "strength_variance", field.strengthVariance },

		{ "grid_size_x", field.gridSize[0] },
		{ "grid_size_y", field.gridSize[1] },
		{ "grid_size_z", field.gridSize[2] },
		{ "grid_direction", field.directionGrid },
		{ "grid_strength", field.strengthGrid }
	};
}
void to_json(nlohmann::ordered_json& j, const ForceField::VectorField& field) {
	j = nlohmann::ordered_json{
		{ "resource_id", field.resourceId },
		{ "filter", field.filter },

		{ "tightness", field.tightness }
	};
}
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
		{ "name", forceField.name },
		{ "lifetime_start", forceField.lifetimeStart },
		{ "lifetime_duration", forceField.lifetimeDuration },
		{ "repeat", forceField.repeat },
		{ "position", forceField.position },

		{ "type", forceField.type },
		{ "exclusion_list", forceField.exclusionList },
		{ "size", forceField.size },
		{ "orientation", forceField.orientation },

		{ "strength", forceField.strength },

		{ "attraction_field", forceField.attractionField },
		{ "acceleration_field", forceField.accelerationField },
		{ "vector_field", forceField.vectorField },
		{ "noise_field", forceField.noiseField },
		{ "drag_field", forceField.dragField }
	};
}
void from_json(const nlohmann::ordered_json& j, ForceField::AttractionField& field) {
	field = ForceField::AttractionField();
}
void from_json(const nlohmann::ordered_json& j, ForceField::AccelerationField& field) {
	field = ForceField::AccelerationField();

	fromJson(field.direction, j, "direction");
	fromJson(field.directionVariance, j, "direction_variance");
	fromJson(field.strengthVariance, j, "strength_variance");

	fromJson(field.gridSize[0], j, "grid_size_x");
	fromJson(field.gridSize[1], j, "grid_size_y");
	fromJson(field.gridSize[2], j, "grid_size_z");
	fromJson(field.directionGrid, j, "grid_direction");
	fromJson(field.strengthGrid, j, "grid_strength");

	std::size_t numGridCells = static_cast<std::size_t>(
		std::max(field.gridSize[0] * field.gridSize[1] * field.gridSize[2], 0));
	field.directionGrid.resize(numGridCells);
	field.strengthGrid.resize(numGridCells);
}
void from_json(const nlohmann::ordered_json& j, ForceField::VectorField& field) {
	field = ForceField::VectorField();

	fromJson(field.resourceId, j, "resource_id");
	fromJson(field.filter, j, "filter");

	fromJson(field.tightness, j, "tightness");
}
void from_json(const nlohmann::ordered_json& j, ForceField::NoiseField& field) {
	field = ForceField::NoiseField();

	fromJson(field.octaves, j, "octaves");
	fromJson(field.frequency, j, "frequency");
	fromJson(field.persistence, j, "persistence");
	fromJson(field.lacunarity, j, "lacunarity");

	fromJson(field.animated, j, "animated");
	fromJson(field.animationTimeScale, j, "animation_time_scale");
	fromJson(field.animationTimeBase, j, "animation_time_base");
}
void from_json(const nlohmann::ordered_json& j, ForceField::DragField& field) {
	field = ForceField::DragField();

	fromJson(field.velocityInfluence, j, "velocity_influence");
	fromJson(field.sizeInfluence, j, "size_influence");
}
void from_json(const nlohmann::ordered_json& j, ForceField& forceField) {
	forceField = ForceField();

	fromJson(forceField.id, j, "id");
	fromJson(forceField.parentId, j, "parent_id");
	fromJson(forceField.name, j, "name");
	fromJson(forceField.lifetimeStart, j, "lifetime_start");
	fromJson(forceField.lifetimeDuration, j, "lifetime_duration");
	fromJson(forceField.repeat, j, "repeat");
	fromJson(forceField.position, j, "position");

	fromJson(forceField.type, j, "type");
	fromJson(forceField.exclusionList, j, "exclusion_list");
	fromJson(forceField.size, j, "size");
	fromJson(forceField.orientation, j, "orientation");

	fromJson(forceField.strength, j, "strength");

	fromJson(forceField.attractionField, j, "attraction_field");
	fromJson(forceField.accelerationField, j, "acceleration_field");
	fromJson(forceField.vectorField, j, "vector_field");
	fromJson(forceField.noiseField, j, "noise_field");
	fromJson(forceField.dragField, j, "drag_field");
}
}