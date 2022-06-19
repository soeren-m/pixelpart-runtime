#include "Collider.h"
#include "JSONUtil.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const Collider& collider) {
	j = nlohmann::ordered_json{
		{ "id", toJson(collider.id) },
		{ "parent_id", toJson(collider.parentId) },
		{ "name", collider.name },
		{ "lifetime_start", collider.lifetimeStart },
		{ "lifetime_duration", collider.lifetimeDuration },
		{ "repeat", collider.repeat },
		{ "emitter_mask", collider.emitterMask },
		{ "points", collider.points },
		{ "bounce", collider.bounce },
		{ "friction", collider.friction }
	};
}
void from_json(const nlohmann::ordered_json& j, Collider& collider) {
	collider = Collider();
	fromJson(collider.id, j, "id", "");
	fromJson(collider.parentId, j, "parent_id", "");
	fromJson(collider.name, j, "name", "");
	fromJson(collider.lifetimeStart, j, "lifetime_start", "delay");
	fromJson(collider.lifetimeDuration, j, "lifetime_duration", "duration");
	fromJson(collider.repeat, j, "repeat", "");
	fromJson(collider.emitterMask, j, "emitter_mask", "");
	fromJson(collider.bounce, j, "bounce", "");
	fromJson(collider.friction, j, "friction", "");

	if(j.contains("p1") && j.contains("p2")) {
		collider.points = std::vector<vec2d>{ 
			j.at("p1").get<vec2d>(),
			j.at("p2").get<vec2d>()
		};
	}
	else {
		fromJson(collider.points, j, "points", "");
	}
}
}