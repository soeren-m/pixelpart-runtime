#include "LightSource.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const LightSource& lightSource) {
	j = nlohmann::ordered_json{
		{ "id", toJson(lightSource.id) },
		{ "parent_id", toJson(lightSource.parentId) },
		{ "name", lightSource.name },
		{ "lifetime_start", lightSource.lifetimeStart },
		{ "lifetime_duration", lightSource.lifetimeDuration },
		{ "repeat", lightSource.repeat },
		{ "position", lightSource.position },

		{ "type", lightSource.type },

		{ "direction", lightSource.direction },
		{ "range", lightSource.range },
		{ "attenuation", lightSource.attenuation },
		{ "spot_angle", lightSource.spotAngle },
		{ "spot_angle_attenuation", lightSource.spotAngleAttenuation },

		{ "color", lightSource.color },
		{ "intensity", lightSource.intensity }
	};
}
void from_json(const nlohmann::ordered_json& j, LightSource& lightSource) {
	lightSource = LightSource();

	fromJson(lightSource.id, j, "id");
	fromJson(lightSource.parentId, j, "parent_id");
	fromJson(lightSource.name, j, "name");
	fromJson(lightSource.lifetimeStart, j, "lifetime_start");
	fromJson(lightSource.lifetimeDuration, j, "lifetime_duration");
	fromJson(lightSource.repeat, j, "repeat");
	fromJson(lightSource.position, j, "position");

	fromJson(lightSource.type, j, "type");

	fromJson(lightSource.direction, j, "direction");
	fromJson(lightSource.range, j, "range");
	fromJson(lightSource.attenuation, j, "attenuation");
	fromJson(lightSource.spotAngle, j, "spot_angle");
	fromJson(lightSource.spotAngleAttenuation, j, "spot_angle_attenuation");

	fromJson(lightSource.color, j, "color");
	fromJson(lightSource.intensity, j, "intensity");
}
}