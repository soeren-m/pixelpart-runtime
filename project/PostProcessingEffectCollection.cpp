#include "PostProcessingEffectCollection.h"
#include "../common/JsonUtil.h"

namespace pixelpart {
PostProcessingEffectCollection::PostProcessingEffectCollection() {

}
PostProcessingEffectCollection::PostProcessingEffectCollection(const std::vector<PostProcessingEffectType>& effectTypes) :
	effects(effectTypes) {

}

const PostProcessingEffectType* PostProcessingEffectCollection::getEffectType(const std::string& typeName) const {
	for(const PostProcessingEffectType& effectType : effects) {
		if(effectType.name == typeName) {
			return &effectType;
		}
	}

	return nullptr;
}
const std::vector<PostProcessingEffectType>& PostProcessingEffectCollection::getEffectTypes() const {
	return effects;
}

void to_json(nlohmann::ordered_json& j, const PostProcessingEffectCollection& effectCollection) {
	j = nlohmann::ordered_json{
		{ "effects", effectCollection.getEffectTypes() }
	};
}
void from_json(const nlohmann::ordered_json& j, PostProcessingEffectCollection& effectCollection) {
	std::vector<PostProcessingEffectType> effects;
	fromJson(effects, j, "effects");

	effectCollection = PostProcessingEffectCollection(effects);
}
}