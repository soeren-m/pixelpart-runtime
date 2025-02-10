#pragma once

#include "ParticleSortCriterion.h"
#include "../common/Types.h"
#include "../json/json.hpp"
#include <string>

namespace pixelpart {
struct ParticleSpriteRendererSettings {
	ParticleSortCriterion sortCriterion = ParticleSortCriterion::none;
};

struct ParticleTrailRendererSettings {
	enum class SmoothingMethod : uint32_t {
		none = 0,
		spline = 1
	};
	enum class TextureRotation : uint32_t {
		up = 0,
		left = 1,
		down = 2,
		right = 3
	};

	SmoothingMethod smoothingMethod = SmoothingMethod::none;
	uint32_t smoothingSegmentCount = 100u;
	TextureRotation textureRotation = TextureRotation::up;
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
NLOHMANN_JSON_SERIALIZE_ENUM(ParticleTrailRendererSettings::TextureRotation, {
	{ ParticleTrailRendererSettings::TextureRotation::up, "up" },
	{ ParticleTrailRendererSettings::TextureRotation::left, "left" },
	{ ParticleTrailRendererSettings::TextureRotation::down, "down" },
	{ ParticleTrailRendererSettings::TextureRotation::right, "right" }
})

void to_json(nlohmann::ordered_json& j, const ParticleSpriteRendererSettings& rendererSettings);
void to_json(nlohmann::ordered_json& j, const ParticleTrailRendererSettings& rendererSettings);
void to_json(nlohmann::ordered_json& j, const ParticleMeshRendererSettings& rendererSettings);
void from_json(const nlohmann::ordered_json& j, ParticleSpriteRendererSettings& rendererSettings);
void from_json(const nlohmann::ordered_json& j, ParticleTrailRendererSettings& rendererSettings);
void from_json(const nlohmann::ordered_json& j, ParticleMeshRendererSettings& rendererSettings);
}