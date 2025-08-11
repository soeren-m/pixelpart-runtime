#include "ImageEffectCollection.h"

namespace pixelpart {
ImageEffectCollection::ImageEffectCollection(const std::vector<ImageEffectType>& effectTypes) :
	effects(effectTypes) {

}

const ImageEffectType* ImageEffectCollection::effectType(const std::string& typeName) const {
	for(const ImageEffectType& effectType : effects) {
		if(effectType.name == typeName) {
			return &effectType;
		}
	}

	return nullptr;
}

const std::vector<ImageEffectType>& ImageEffectCollection::effectTypes() const {
	return effects;
}

void to_json(nlohmann::ordered_json& j, const ImageEffectCollection& effectCollection) {
	j = nlohmann::ordered_json{
		{ "effects", effectCollection.effectTypes() }
	};
}
void from_json(const nlohmann::ordered_json& j, ImageEffectCollection& effectCollection) {
	effectCollection = ImageEffectCollection(
		j.value("effects", std::vector<ImageEffectType>()));
}
}
