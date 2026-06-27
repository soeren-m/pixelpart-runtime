#pragma once

#include "../types/Types.h"
#include "../json/json.hpp"
#include <cstdint>

namespace pixelpart {
struct RenderSettings {
	std::uint32_t simulationSeed = 0;

	std::uint32_t frameWidth = 1;
	std::uint32_t frameHeight = 1;

	float_t simulationRate = 60.0;
	float_t frameRate = 60.0;
	float_t duration = 1.0;
	float_t delay = 0.0;
	float_t playbackSpeed = 1.0;

	bool loop = false;
	bool backgroundEnabled = false;
	bool postProcessingEnabled = true;
	bool seamlessRenderingEnabled = false;
	float4_t backgroundColor = float4_t(0.0, 0.0, 0.0, 1.0);

	float3_t ambientLighting = float3_t(0.1);
	bool hdrEnabled = true;
	float_t hdrBloomIntensity = 0.1;
	float_t hdrExposure = 1.0;
};

void to_json(nlohmann::ordered_json& j, const RenderSettings& settings);
void from_json(const nlohmann::ordered_json& j, RenderSettings& settings);
}
