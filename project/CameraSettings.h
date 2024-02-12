#pragma once

#include "../common/Types.h"

namespace pixelpart {
struct CameraSettings {
	vec3_t position = vec3_t(0.0, 0.0, 1.0);
	float_t zoom = 1.0;
	float_t fieldOfView = 60.0;
	float_t pitch = 0.0;
	float_t yaw = 0.0;
	float_t roll = 0.0;
};

void to_json(nlohmann::ordered_json& j, const CameraSettings& settings);
void from_json(const nlohmann::ordered_json& j, CameraSettings& settings);
}