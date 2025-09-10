#pragma once

#include "../common/Math.h"

namespace pixelpart {
struct SceneContext {
	float3_t effectScale = float3_t(1.0);

	float3_t cameraPosition = float3_t(0.0, 0.0, 0.0);
	float3_t cameraForward = float3_t(0.0, 0.0, 1.0);
	float3_t cameraRight = float3_t(1.0, 0.0, 0.0);
	float3_t cameraUp = float3_t(0.0, 1.0, 0.0);
};
}
