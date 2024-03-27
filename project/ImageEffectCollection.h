#pragma once

#include "ImageEffectType.h"

namespace pixelpart {
class ImageEffectCollection {
public:
	ImageEffectCollection();
	ImageEffectCollection(const std::vector<ImageEffectType>& effectTypes);

	const ImageEffectType* getEffectType(const std::string& typeName) const;
	const std::vector<ImageEffectType>& getEffectTypes() const;

private:
	std::vector<ImageEffectType> effects;
};

void to_json(nlohmann::ordered_json& j, const ImageEffectCollection& effectCollection);
void from_json(const nlohmann::ordered_json& j, ImageEffectCollection& effectCollection);
}