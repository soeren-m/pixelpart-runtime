#include "ParticleRendererSettings.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const ParticleSpriteRendererSettings& rendererSettings) {
	j = nlohmann::ordered_json{
		{ "sort_criterion", rendererSettings.sortCriterion }
	};
}
void to_json(nlohmann::ordered_json& j, const ParticleTrailRendererSettings& rendererSettings) {
	j = nlohmann::ordered_json{
		{ "smoothing_method", rendererSettings.smoothingMethod },
		{ "smoothing_segments", rendererSettings.smoothingSegmentCount },
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
	rendererSettings.sortCriterion = j.value("sort_criterion", ParticleSortCriterion::none);
}
void from_json(const nlohmann::ordered_json& j, ParticleTrailRendererSettings& rendererSettings) {
	rendererSettings = ParticleTrailRendererSettings();
	rendererSettings.smoothingMethod = j.value("smoothing_method", ParticleTrailRendererSettings::SmoothingMethod::none);
	rendererSettings.smoothingSegmentCount = j.value("smoothing_segments", 100u);
	rendererSettings.textureRotation = j.value("texture_rotation", ParticleTrailRendererSettings::TextureRotation::up);
	rendererSettings.textureUVFactor = j.value("texture_uv_factor", 1.0);
}
void from_json(const nlohmann::ordered_json& j, ParticleMeshRendererSettings& rendererSettings) {
	rendererSettings = ParticleMeshRendererSettings();
	rendererSettings.meshResourceId = j.value("mesh_resource", "");
	rendererSettings.sortCriterion = j.value("sort_criterion", ParticleSortCriterion::none);
}
}
