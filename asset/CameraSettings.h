#pragma once

#include "../common/Types.h"
#include "../common/Math.h"
#include "../json/json.hpp"

namespace pixelpart {
struct CameraSettings {
	float3_t position = float3_t(0.0, 0.0, 1.0);
	float_t zoom = 1.0;
	float_t fieldOfView = 60.0;
	float_t pitch = 0.0;
	float_t yaw = 0.0;
	float_t roll = 0.0;
};

void to_json(nlohmann::ordered_json& j, const CameraSettings& settings);
void from_json(const nlohmann::ordered_json& j, CameraSettings& settings);
}