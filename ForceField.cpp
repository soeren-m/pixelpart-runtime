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
		{ "type", forceField.type },
		{ "emitter_mask", forceField.emitterMask },
		{ "position", forceField.motionPath },
		{ "width", forceField.width },
		{ "height", forceField.height },
		{ "orientation", forceField.orientation },
		{ "direction", forceField.direction },
		{ "strength", forceField.strength },
		{ "strength_variance", forceField.strengthVariance },
		{ "direction_variance", forceField.directionVariance },
		{ "grid_size_x", forceField.gridSize[0] },
		{ "grid_size_y", forceField.gridSize[1] },
		{ "grid_direction", forceField.directionGrid },
		{ "grid_strength", forceField.strengthGrid },
	};
}
void from_json(const nlohmann::ordered_json& j, ForceField& forceField) {
	forceField = ForceField();
	fromJson(forceField.id, j, "id", "");
	fromJson(forceField.parentId, j, "parent_id", "");
	fromJson(forceField.name, j, "name", "");
	fromJson(forceField.lifetimeStart, j, "lifetime_start", "delay");
	fromJson(forceField.lifetimeDuration, j, "lifetime_duration", "duration");
	fromJson(forceField.repeat, j, "repeat", "");
	fromJson(forceField.type, j, "type", "");
	fromJson(forceField.emitterMask, j, "emitter_mask", "");
	fromJson(forceField.motionPath, j, "position", "");

	if(j.contains("size")) {
		vec2d size = j.at("size");
		forceField.width = Curve<floatd>(size.x);
		forceField.height = Curve<floatd>(size.y);
	}
	else {
		fromJson(forceField.width, j, "width", "");
		fromJson(forceField.height, j, "height", "");
	}

	fromJson(forceField.orientation, j, "orientation", "");
	fromJson(forceField.direction, j, "direction", "");
	fromJson(forceField.strength, j, "strength", "");
	fromJson(forceField.strengthVariance, j, "strength_variance", "");
	fromJson(forceField.directionVariance, j, "direction_variance", "");
	fromJson(forceField.gridSize[0], j, "grid_size_x", "");
	fromJson(forceField.gridSize[1], j, "grid_size_y", "");
	fromJson(forceField.directionGrid, j, "grid_direction", "");
	fromJson(forceField.strengthGrid, j, "grid_strength", "");

	if(forceField.directionGrid.size() != forceField.gridSize[0] * forceField.gridSize[1]) {
		forceField.directionGrid.resize(forceField.gridSize[0] * forceField.gridSize[1]);
	}
	if(forceField.strengthGrid.size() != forceField.gridSize[0] * forceField.gridSize[1]) {
		forceField.strengthGrid.resize(forceField.gridSize[0] * forceField.gridSize[1]);
	}
}
}