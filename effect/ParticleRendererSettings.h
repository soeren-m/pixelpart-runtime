#pragma once

#include "ParticleSortCriterion.h"

namespace pixelpart {
struct ParticleSpriteRendererSettings {
	ParticleSortCriterion sortCriterion = ParticleSortCriterion::none;
	uint32_t sortPasses = 100;
};

struct ParticleTrailRendererSettings {
	floatd textureUVFactor = 1.0;
	uint32_t textureRotation = 0;
	uint32_t numTrailSegments = 0;
};

void to_json(nlohmann::ordered_json& j, const ParticleSpriteRendererSettings& rendererSettings);
void to_json(nlohmann::ordered_json& j, const ParticleTrailRendererSettings& rendererSettings);
void from_json(const nlohmann::ordered_json& j, ParticleSpriteRendererSettings& rendererSettings);
void from_json(const nlohmann::ordered_json& j, ParticleTrailRendererSettings& rendererSettings);
}