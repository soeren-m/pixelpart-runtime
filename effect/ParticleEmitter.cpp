#include "ParticleEmitter.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const ParticleEmitter& particleEmitter) {
	j = nlohmann::ordered_json{
		{ "id", toJson(particleEmitter.id) },
		{ "parent_id", toJson(particleEmitter.parentId) },
		{ "name", particleEmitter.name },
		{ "lifetime_start", particleEmitter.lifetimeStart },
		{ "lifetime_duration", particleEmitter.lifetimeDuration },
		{ "repeat", particleEmitter.repeat },
		{ "position", particleEmitter.position },

		{ "shape", particleEmitter.shape },
		{ "path", particleEmitter.path },
		{ "size", particleEmitter.size },
		{ "orientation", particleEmitter.orientation },

		{ "distribution", particleEmitter.distribution },
		{ "grid_order", particleEmitter.gridOrder },
		{ "grid_size_x", particleEmitter.gridSize[0] },
		{ "grid_size_y", particleEmitter.gridSize[1] },
		{ "grid_size_z", particleEmitter.gridSize[2] },
		{ "emission_mode", particleEmitter.emissionMode },
		{ "direction_mode", particleEmitter.directionMode },
		{ "direction", particleEmitter.direction },
		{ "spread", particleEmitter.spread },
	};
}
void from_json(const nlohmann::ordered_json& j, ParticleEmitter& particleEmitter) {
	particleEmitter = ParticleEmitter();

	fromJson(particleEmitter.id, j, "id");
	fromJson(particleEmitter.parentId, j, "parent_id");
	fromJson(particleEmitter.name, j, "name");
	fromJson(particleEmitter.lifetimeStart, j, "lifetime_start");
	fromJson(particleEmitter.lifetimeDuration, j, "lifetime_duration");
	fromJson(particleEmitter.repeat, j, "repeat");
	fromJson(particleEmitter.position, j, "position");

	fromJson(particleEmitter.shape, j, "shape");
	fromJson(particleEmitter.path, j, "path");
	fromJson(particleEmitter.size, j, "size");
	fromJson(particleEmitter.orientation, j, "orientation");

	fromJson(particleEmitter.distribution, j, "distribution");
	fromJson(particleEmitter.gridOrder, j, "grid_order");
	fromJson(particleEmitter.gridSize[0], j, "grid_size_x");
	fromJson(particleEmitter.gridSize[1], j, "grid_size_y");
	fromJson(particleEmitter.gridSize[2], j, "grid_size_z");
	fromJson(particleEmitter.emissionMode, j, "emission_mode");
	fromJson(particleEmitter.directionMode, j, "direction_mode");
	fromJson(particleEmitter.direction, j, "direction");
	fromJson(particleEmitter.spread, j, "spread");
}
}