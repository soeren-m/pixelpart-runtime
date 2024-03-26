#include "RenderSettings.h"
#include "../common/Json.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const RenderSettings& settings) {
	j = nlohmann::ordered_json{
		{ "simulation_seed", settings.simulationSeed },

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
		{ "background_color", settings.backgroundColor },

		{ "ambient_lighting", settings.ambientLighting },
		{ "hdr_enabled", settings.hdrEnabled },
		{ "hdr_bloom_intensity", settings.hdrBloomIntensity },
		{ "hdr_exposure", settings.hdrExposure }
	};
}

void from_json(const nlohmann::ordered_json& j, RenderSettings& settings) {
	settings = RenderSettings();

	fromJson(settings.simulationSeed, j, "simulation_seed");

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
	fromJson(settings.backgroundColor, j, "background_color");

	fromJson(settings.ambientLighting, j, "ambient_lighting");
	fromJson(settings.hdrEnabled, j, "hdr_enabled");
	fromJson(settings.hdrBloomIntensity, j, "hdr_bloom_intensity");
	fromJson(settings.hdrExposure, j, "hdr_exposure");
}
}