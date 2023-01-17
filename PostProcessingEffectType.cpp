#include "PostProcessingEffectType.h"
#include "JSONUtil.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const PostProcessingEffectType& effectType) {
	j = nlohmann::ordered_json{
		{ "name", effectType.name },
		{ "category", effectType.category },
		{ "code", effectType.code },
		{ "parameters", effectType.parameters }
	};
}
void from_json(const nlohmann::ordered_json& j, PostProcessingEffectType& effectType) {
	effectType = PostProcessingEffectType();

	fromJson(effectType.name, j, "name");
	fromJson(effectType.category, j, "category");
	fromJson(effectType.code, j, "code");
	fromJson(effectType.parameters, j, "parameters");
}
}