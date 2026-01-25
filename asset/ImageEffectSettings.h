#pragma once

#include "ImageEffect.h"
#include "../json/json.hpp"
#include <vector>

namespace pixelpart {
struct ImageEffectSettings {
	std::vector<ImageEffect> effects;
};

void to_json(nlohmann::ordered_json& j, const ImageEffectSettings& settings);
void from_json(const nlohmann::ordered_json& j, ImageEffectSettings& settings);
}
