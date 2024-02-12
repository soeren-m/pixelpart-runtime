#pragma once

#include "ParticleSortCriterion.h"

namespace pixelpart {
struct ParticleSpriteRendererSettings {
	ParticleSortCriterion sortCriterion = ParticleSortCriterion::none;
};

struct ParticleTrailRendererSettings {
	enum class SmoothingMethod : uint32_t {
		none = 0,
		spline = 1
	};

	SmoothingMethod smoothingMethod = SmoothingMethod::none;
	uint32_t numSmoothingSegments = 100u;

	float_t textureUVFactor = 1.0;
	uint32_t textureRotation = 0u;
};

struct ParticleMeshRendererSettings {
	std::string meshResourceId;
};

NLOHMANN_JSON_SERIALIZE_ENUM(ParticleTrailRendererSettings::SmoothingMethod, {
	{ ParticleTrailRendererSettings::SmoothingMethod::none, "none" },
	{ ParticleTrailRendererSettings::SmoothingMethod::spline, "spline" }
})

void to_json(nlohmann::ordered_json& j, const ParticleSpriteRendererSettings& rendererSettings);
void to_json(nlohmann::ordered_json& j, const ParticleTrailRendererSettings& rendererSettings);
void to_json(nlohmann::ordered_json& j, const ParticleMeshRendererSettings& rendererSettings);
void from_json(const nlohmann::ordered_json& j, ParticleSpriteRendererSettings& rendererSettings);
void from_json(const nlohmann::ordered_json& j, ParticleTrailRendererSettings& rendererSettings);
void from_json(const nlohmann::ordered_json& j, ParticleMeshRendererSettings& rendererSettings);
}