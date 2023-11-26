#pragma once

#include "../common/Types.h"

namespace pixelpart {
struct CameraSettings {
	vec3d position = vec3d(0.0, 0.0, 1.0);
	floatd zoom = 1.0;
	floatd fieldOfView = 60.0;
	floatd pitch = 0.0;
	floatd yaw = 0.0;
	floatd roll = 0.0;
};

void to_json(nlohmann::ordered_json& j, const CameraSettings& settings);
void from_json(const nlohmann::ordered_json& j, CameraSettings& settings);
}