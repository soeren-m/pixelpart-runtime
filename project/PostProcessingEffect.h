#pragma once

#include "PostProcessingEffectType.h"

namespace pixelpart {
struct PostProcessingEffect {
	std::string type;
	std::vector<ShaderParameter::Value> parameters;

	PostProcessingEffect();
	PostProcessingEffect(const PostProcessingEffectType& effectType);
};

void to_json(nlohmann::ordered_json& j, const PostProcessingEffect& effect);
void from_json(const nlohmann::ordered_json& j, PostProcessingEffect& effect);
}