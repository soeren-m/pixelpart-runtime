#include "Collider.h"
#include "../common/JsonUtil.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const Collider& collider) {
	j = nlohmann::ordered_json{
		{ "id", toJson(collider.id) },
		{ "parent_id", toJson(collider.parentId) },
		{ "name", collider.name },
		{ "lifetime_start", collider.lifetimeStart },
		{ "lifetime_duration", collider.lifetimeDuration },
		{ "repeat", collider.repeat },
		{ "position", collider.position },

		{ "exclusion_list", collider.exclusionList },
		{ "points", collider.points },

		{ "bounce", collider.bounce },
		{ "friction", collider.friction }
	};
}
void from_json(const nlohmann::ordered_json& j, Collider& collider) {
	collider = Collider();

	fromJson(collider.id, j, "id");
	fromJson(collider.parentId, j, "parent_id");
	fromJson(collider.name, j, "name");
	fromJson(collider.lifetimeStart, j, "lifetime_start");
	fromJson(collider.lifetimeDuration, j, "lifetime_duration");
	fromJson(collider.repeat, j, "repeat");
	fromJson(collider.position, j, "position");

	fromJson(collider.exclusionList, j, "exclusion_list");
	fromJson(collider.points, j, "points");

	fromJson(collider.bounce, j, "bounce");
	fromJson(collider.friction, j, "friction");
}
}