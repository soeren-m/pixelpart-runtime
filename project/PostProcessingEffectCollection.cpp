#include "PostProcessingEffectCollection.h"
#include "../common/StringUtil.h"
#include "../common/JsonUtil.h"

namespace pixelpart {
PostProcessingEffectCollection::PostProcessingEffectCollection() {

}
PostProcessingEffectCollection::PostProcessingEffectCollection(const std::vector<PostProcessingEffectType>& effectTypes, const std::string& shTemplate) :
	effects(effectTypes), shaderTemplate(shTemplate) {

}

std::string PostProcessingEffectCollection::build(const std::string& typeName) const {
	const PostProcessingEffectType* effectType = getEffectType(typeName);
	if(effectType == nullptr) {
		return std::string();
	}

	std::string code = shaderTemplate;
	code = replace(code, effectType->code, "{main}");

	return code;
}

const PostProcessingEffectType* PostProcessingEffectCollection::getEffectType(const std::string& typeName) const {
	for(std::size_t i = 0u; i < effects.size(); i++) {
		if(effects[i].name == typeName) {
			return &(effects.at(i));
		}
	}

	return nullptr;
}
const std::vector<PostProcessingEffectType>& PostProcessingEffectCollection::getEffectTypes() const {
	return effects;
}

const std::string& PostProcessingEffectCollection::getShaderTemplate() const {
	return shaderTemplate;
}

void to_json(nlohmann::ordered_json& j, const PostProcessingEffectCollection& effectCollection) {
	j = nlohmann::ordered_json{
		{ "effects", effectCollection.getEffectTypes() },
		{ "shader_template", effectCollection.getShaderTemplate() }
	};
}
void from_json(const nlohmann::ordered_json& j, PostProcessingEffectCollection& effectCollection) {
	std::vector<PostProcessingEffectType> effects;
	fromJson(effects, j, "effects");

	std::string shaderTemplate;
	fromJson(shaderTemplate, j, "shader_template");

	effectCollection = PostProcessingEffectCollection(effects, shaderTemplate);
}
}