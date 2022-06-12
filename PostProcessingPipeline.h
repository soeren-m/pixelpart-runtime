#pragma once

#include "PostProcessingEffect.h"
#include "PostProcessingEffectCollection.h"

namespace pixelpart {
struct PostProcessingPipeline {
	static PostProcessingEffectCollection effectCollection;

	std::vector<PostProcessingEffect> effects;
};

void to_json(nlohmann::ordered_json& j, const PostProcessingPipeline& pipeline);
void from_json(const nlohmann::ordered_json& j, PostProcessingPipeline& pipeline);
}