#pragma once

#include "../common/Types.h"

namespace pixelpart {
struct RenderSettings {
	uint32_t simulationSeed = 0u;

	uint32_t frameWidth = 1u;
	uint32_t frameHeight = 1u;

	float_t captureRate = 30.0;
	float_t duration = 1.0;
	float_t delay = 0.0;
	float_t playbackSpeed = 1.0;

	bool loop = false;
	bool backgroundEnabled = false;
	bool postProcessingEnabled = true;
	bool seamlessRenderingEnabled = false;
	vec4_t backgroundColor = vec4_t(0.0, 0.0, 0.0, 1.0);

	vec3_t ambientLighting = vec3_t(0.1);
	bool hdrEnabled = true;
	float_t hdrBloomIntensity = 0.1;
	float_t hdrExposure = 1.0;
};

void to_json(nlohmann::ordered_json& j, const RenderSettings& settings);
void from_json(const nlohmann::ordered_json& j, RenderSettings& settings);
}