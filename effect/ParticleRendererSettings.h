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
	enum class TextureOrientation : uint32_t {
		top = 0,
		// TODO
	};

	SmoothingMethod smoothingMethod = SmoothingMethod::none;
	uint32_t numSmoothingSegments = 100u;

	TextureOrientation textureOrientation = TextureOrientation::top;
	float_t textureUVFactor = 1.0;
};

struct ParticleMeshRendererSettings {
	std::string meshResourceId;

	ParticleSortCriterion sortCriterion = ParticleSortCriterion::none;
};

NLOHMANN_JSON_SERIALIZE_ENUM(ParticleTrailRendererSettings::SmoothingMethod, {
	{ ParticleTrailRendererSettings::SmoothingMethod::none, "none" },
	{ ParticleTrailRendererSettings::SmoothingMethod::spline, "spline" }
})
NLOHMANN_JSON_SERIALIZE_ENUM(ParticleTrailRendererSettings::TextureOrientation, {
	{ ParticleTrailRendererSettings::TextureOrientation::top, "top" },
})

void to_json(nlohmann::ordered_json& j, const ParticleSpriteRendererSettings& rendererSettings);
void to_json(nlohmann::ordered_json& j, const ParticleTrailRendererSettings& rendererSettings);
void to_json(nlohmann::ordered_json& j, const ParticleMeshRendererSettings& rendererSettings);
void from_json(const nlohmann::ordered_json& j, ParticleSpriteRendererSettings& rendererSettings);
void from_json(const nlohmann::ordered_json& j, ParticleTrailRendererSettings& rendererSettings);
void from_json(const nlohmann::ordered_json& j, ParticleMeshRendererSettings& rendererSettings);
}