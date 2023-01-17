#include "ForceField.h"
#include "JSONUtil.h"

namespace pixelpart {
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

		{ "direction", forceField.direction },
		{ "strength", forceField.strength },
		{ "direction_variance", forceField.directionVariance },
		{ "strength_variance", forceField.strengthVariance },

		{ "grid_size_x", forceField.gridSize[0] },
		{ "grid_size_y", forceField.gridSize[1] },
		{ "grid_size_z", forceField.gridSize[2] },
		{ "grid_direction", forceField.directionGrid },
		{ "grid_strength", forceField.strengthGrid },
	};
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

	fromJson(forceField.direction, j, "direction");
	fromJson(forceField.strength, j, "strength");
	fromJson(forceField.directionVariance, j, "direction_variance");
	fromJson(forceField.strengthVariance, j, "strength_variance");

	fromJson(forceField.gridSize[0], j, "grid_size_x");
	fromJson(forceField.gridSize[1], j, "grid_size_y");
	fromJson(forceField.gridSize[2], j, "grid_size_z");
	fromJson(forceField.directionGrid, j, "grid_direction");
	fromJson(forceField.strengthGrid, j, "grid_strength");

	std::size_t numGridCells = forceField.gridSize[0] * forceField.gridSize[1] * forceField.gridSize[2];
	forceField.directionGrid.resize(numGridCells);
	forceField.strengthGrid.resize(numGridCells);
}
}