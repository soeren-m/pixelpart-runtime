#include "RenderSettings.h"
#include "../common/JsonUtil.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const RenderSettings& settings) {
	j = nlohmann::ordered_json{
		{ "frame_width", settings.frameWidth },
		{ "frame_height", settings.frameHeight },
		{ "capture_rate", settings.captureRate },
		{ "duration", settings.duration },
		{ "delay", settings.delay },
		{ "playback_speed", settings.playbackSpeed },
		{ "loop", settings.loop },
		{ "background_enabled", settings.backgroundEnabled },
		{ "postprocessing_enabled", settings.postProcessingEnabled },
		{ "seamlessrendering_enabled", settings.seamlessRenderingEnabled },
		{ "simulation_seed", settings.simulationSeed }
	};
}

void from_json(const nlohmann::ordered_json& j, RenderSettings& settings) {
	settings = RenderSettings();

	fromJson(settings.frameWidth, j, "frame_width");
	fromJson(settings.frameHeight, j, "frame_height");
	fromJson(settings.captureRate, j, "capture_rate");
	fromJson(settings.duration, j, "duration");
	fromJson(settings.delay, j, "delay");
	fromJson(settings.playbackSpeed, j, "playback_speed");
	fromJson(settings.loop, j, "loop", "looping");
	fromJson(settings.backgroundEnabled, j, "background_enabled");
	fromJson(settings.postProcessingEnabled, j, "postprocessing_enabled");
	fromJson(settings.seamlessRenderingEnabled, j, "seamlessrendering_enabled");
	fromJson(settings.simulationSeed, j, "simulation_seed");
}
}