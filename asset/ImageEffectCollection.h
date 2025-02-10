#pragma once

#include "ImageEffectType.h"
#include "../json/json.hpp"
#include <string>
#include <vector>

namespace pixelpart {
class ImageEffectCollection {
public:
	ImageEffectCollection() = default;
	ImageEffectCollection(const std::vector<ImageEffectType>& effectTypes);

	const ImageEffectType* effectType(const std::string& typeName) const;

	const std::vector<ImageEffectType>& effectTypes() const;

private:
	std::vector<ImageEffectType> effects;
};

void to_json(nlohmann::ordered_json& j, const ImageEffectCollection& effectCollection);
void from_json(const nlohmann::ordered_json& j, ImageEffectCollection& effectCollection);
}