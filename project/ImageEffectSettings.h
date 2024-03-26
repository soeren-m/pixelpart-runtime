#pragma once

#include "ImageEffect.h"
#include "ImageEffectCollection.h"

namespace pixelpart {
struct ImageEffectSettings {
	static ImageEffectCollection effectCollection;

	std::vector<ImageEffect> effects;
};

void to_json(nlohmann::ordered_json& j, const ImageEffectSettings& settings);
void from_json(const nlohmann::ordered_json& j, ImageEffectSettings& settings);
}