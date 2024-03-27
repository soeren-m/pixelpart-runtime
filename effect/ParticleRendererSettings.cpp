#include "ParticleRendererSettings.h"
#include "../common/Json.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const ParticleSpriteRendererSettings& rendererSettings) {
	j = nlohmann::ordered_json{
		{ "sort_criterion", rendererSettings.sortCriterion }
	};
}
void to_json(nlohmann::ordered_json& j, const ParticleTrailRendererSettings& rendererSettings) {
	j = nlohmann::ordered_json{
		{ "smoothing_method", rendererSettings.smoothingMethod },
		{ "smoothing_segments", rendererSettings.numSmoothingSegments },

		{ "texture_rotation", rendererSettings.textureRotation },
		{ "texture_uv_factor", rendererSettings.textureUVFactor }
	};
}
void to_json(nlohmann::ordered_json& j, const ParticleMeshRendererSettings& rendererSettings) {
	j = nlohmann::ordered_json{
		{ "mesh_resource", rendererSettings.meshResourceId },

		{ "sort_criterion", rendererSettings.sortCriterion }
	};
}
void from_json(const nlohmann::ordered_json& j, ParticleSpriteRendererSettings& rendererSettings) {
	rendererSettings = ParticleSpriteRendererSettings();

	fromJson(rendererSettings.sortCriterion, j, "sort_criterion");
}
void from_json(const nlohmann::ordered_json& j, ParticleTrailRendererSettings& rendererSettings) {
	rendererSettings = ParticleTrailRendererSettings();

	fromJson(rendererSettings.smoothingMethod, j, "smoothing_method");
	fromJson(rendererSettings.numSmoothingSegments, j, "smoothing_segments");

	fromJson(rendererSettings.textureRotation, j, "texture_rotation");
	fromJson(rendererSettings.textureUVFactor, j, "texture_uv_factor");
}
void from_json(const nlohmann::ordered_json& j, ParticleMeshRendererSettings& rendererSettings) {
	rendererSettings = ParticleMeshRendererSettings();

	fromJson(rendererSettings.meshResourceId, j, "mesh_resource");

	fromJson(rendererSettings.sortCriterion, j, "sort_criterion");
}
}