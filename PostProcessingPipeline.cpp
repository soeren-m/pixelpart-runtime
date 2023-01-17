#include "PostProcessingPipeline.h"
#include "JSONUtil.h"

namespace pixelpart {
PostProcessingEffectCollection PostProcessingPipeline::effectCollection = PostProcessingEffectCollection();

void to_json(nlohmann::ordered_json& j, const PostProcessingPipeline& pipeline) {
	j = nlohmann::ordered_json{
		{ "effects", pipeline.effects }
	};
}
void from_json(const nlohmann::ordered_json& j, PostProcessingPipeline& pipeline) {
	pipeline = PostProcessingPipeline();

	fromJson(pipeline.effects, j, "effects");
}
}