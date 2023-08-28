#include "ParticleRendererSettings.h"
#include "../common/JsonUtil.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const ParticleSpriteRendererSettings& rendererSettings) {
	j = nlohmann::ordered_json{
		{ "sort_criterion", rendererSettings.sortCriterion },
		{ "sort_passes", rendererSettings.sortPasses }
	};
}
void to_json(nlohmann::ordered_json& j, const ParticleTrailRendererSettings& rendererSettings) {
	j = nlohmann::ordered_json{
		{ "texture_uv_factor", rendererSettings.textureUVFactor },
		{ "texture_rotation", rendererSettings.textureRotation },
		{ "num_trail_segments", rendererSettings.numTrailSegments }
	};
}
void from_json(const nlohmann::ordered_json& j, ParticleSpriteRendererSettings& rendererSettings) {
	rendererSettings = ParticleSpriteRendererSettings();

	fromJson(rendererSettings.sortCriterion, j, "sort_criterion");
	fromJson(rendererSettings.sortPasses, j, "sort_passes");
}
void from_json(const nlohmann::ordered_json& j, ParticleTrailRendererSettings& rendererSettings) {
	rendererSettings = ParticleTrailRendererSettings();

	fromJson(rendererSettings.textureUVFactor, j, "texture_uv_factor");
	fromJson(rendererSettings.textureRotation, j, "texture_rotation");
	fromJson(rendererSettings.numTrailSegments, j, "num_trail_segments");
}
}