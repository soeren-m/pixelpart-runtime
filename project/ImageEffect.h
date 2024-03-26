#pragma once

#include "ImageEffectType.h"

namespace pixelpart {
struct ImageEffect {
	std::string type;
	std::vector<VariantParameter::Value> parameters;
	bool visible = true;

	ImageEffect();
	ImageEffect(const ImageEffectType& effectType);
};

void to_json(nlohmann::ordered_json& j, const ImageEffect& effect);
void from_json(const nlohmann::ordered_json& j, ImageEffect& effect);
}