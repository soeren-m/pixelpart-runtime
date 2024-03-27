#include "CameraSettings.h"
#include "../common/Json.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const CameraSettings& settings) {
	j = nlohmann::ordered_json{
		{ "position", settings.position },
		{ "zoom", settings.zoom },
		{ "fov", settings.fieldOfView },
		{ "pitch", settings.pitch },
		{ "yaw", settings.yaw },
		{ "roll", settings.roll }
	};
}
void from_json(const nlohmann::ordered_json& j, CameraSettings& settings) {
	settings = CameraSettings();

	fromJson(settings.position, j, "position");
	fromJson(settings.zoom, j, "zoom");
	fromJson(settings.fieldOfView, j, "fov");
	fromJson(settings.pitch, j, "pitch");
	fromJson(settings.yaw, j, "yaw");
	fromJson(settings.roll, j, "roll");
}
}