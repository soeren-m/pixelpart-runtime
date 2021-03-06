#pragma once

#include "Types.h"

namespace pixelpart {
struct RenderSettings {
	uint32_t frameWidth = 1;
	uint32_t frameHeight = 1;
	floatd captureRate = 30.0;
	floatd duration = 1.0;
	floatd delay = 0.0;
	floatd playbackSpeed = 1.0;
	bool loop = false;
	bool backgroundEnabled = true;
	bool postProcessingEnabled = true;
	bool seamlessRenderingEnabled = false;
	uint32_t simulationSeed = 0;
};

void to_json(nlohmann::ordered_json& j, const RenderSettings& settings);
void from_json(const nlohmann::ordered_json& j, RenderSettings& settings);
}