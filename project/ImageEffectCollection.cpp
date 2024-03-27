#include "ImageEffectCollection.h"
#include "../common/Json.h"

namespace pixelpart {
ImageEffectCollection::ImageEffectCollection() {

}
ImageEffectCollection::ImageEffectCollection(const std::vector<ImageEffectType>& effectTypes) :
	effects(effectTypes) {

}

const ImageEffectType* ImageEffectCollection::getEffectType(const std::string& typeName) const {
	for(const ImageEffectType& effectType : effects) {
		if(effectType.name == typeName) {
			return &effectType;
		}
	}

	return nullptr;
}
const std::vector<ImageEffectType>& ImageEffectCollection::getEffectTypes() const {
	return effects;
}

void to_json(nlohmann::ordered_json& j, const ImageEffectCollection& effectCollection) {
	j = nlohmann::ordered_json{
		{ "effects", effectCollection.getEffectTypes() }
	};
}
void from_json(const nlohmann::ordered_json& j, ImageEffectCollection& effectCollection) {
	std::vector<ImageEffectType> effects;
	fromJson(effects, j, "effects");

	effectCollection = ImageEffectCollection(effects);
}
}