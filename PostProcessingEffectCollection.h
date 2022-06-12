#pragma once

#include "PostProcessingEffectType.h"

namespace pixelpart {
struct PostProcessingEffectCollection {
	std::vector<PostProcessingEffectType> effects;
	std::string shaderTemplate;

	std::string build(const std::string& typeName) const;

	const PostProcessingEffectType* getEffectType(const std::string& name) const;
};

void to_json(nlohmann::ordered_json& j, const PostProcessingEffectCollection& effectCollection);
void from_json(const nlohmann::ordered_json& j, PostProcessingEffectCollection& effectCollection);
}