#include "CameraSettings.h"

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
	settings.position = j.value("position", float3_t(0.0, 0.0, 1.0));
	settings.zoom = j.value("zoom", 1.0);
	settings.fieldOfView = j.value("fov", 60.0);
	settings.pitch = j.value("pitch", 0.0);
	settings.yaw = j.value("yaw", 0.0);
	settings.roll = j.value("roll", 0.0);
}
}
