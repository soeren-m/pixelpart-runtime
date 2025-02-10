#include "RenderSettings.h"

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

	settings.simulationSeed = j.value("simulation_seed", 0u);

	settings.frameWidth = j.value("frame_width", 1u);
	settings.frameHeight = j.value("frame_height", 1u);

	settings.captureRate = j.value("capture_rate", 30.0);
	settings.duration = j.value("duration", 1.0);
	settings.delay = j.value("delay", 0.0);
	settings.playbackSpeed = j.value("playback_speed", 1.0);

	settings.loop = j.value("loop", false);
	settings.backgroundEnabled = j.value("background_enabled", false);
	settings.postProcessingEnabled = j.value("postprocessing_enabled", true);
	settings.seamlessRenderingEnabled = j.value("seamlessrendering_enabled", false);
	settings.backgroundColor = j.value("background_color", float4_t(0.0, 0.0, 0.0, 1.0));

	settings.ambientLighting = j.value("ambient_lighting", float3_t(0.1));
	settings.hdrEnabled = j.value("hdr_enabled", true);
	settings.hdrBloomIntensity = j.value("hdr_bloom_intensity", 0.1);
	settings.hdrExposure = j.value("hdr_exposure", 1.0);
}
}