#include "PostProcessingEffect.h"
#include "../common/JsonUtil.h"

namespace pixelpart {
PostProcessingEffect::PostProcessingEffect() {

}
PostProcessingEffect::PostProcessingEffect(const PostProcessingEffectType& effectType) : type(effectType.name), parameters(effectType.parameters.size()) {
	for(std::size_t p = 0; p < parameters.size(); p++) {
		parameters[p] = effectType.parameters[p].defaultValue;
	}
}

void to_json(nlohmann::ordered_json& j, const PostProcessingEffect& effect) {
	j = nlohmann::ordered_json{
		{ "type", effect.type },
		{ "parameters", effect.parameters }
	};
}
void from_json(const nlohmann::ordered_json& j, PostProcessingEffect& effect) {
	effect = PostProcessingEffect();

	fromJson(effect.type, j, "type");
	fromJson(effect.parameters, j, "parameters");
}
}