#pragma once

#include "../common/VariantParameter.h"

namespace pixelpart {
struct ImageEffectType {
	std::string name;
	std::string category;
	std::string code;
	std::vector<VariantParameter> parameters;
};

void to_json(nlohmann::ordered_json& j, const ImageEffectType& effectType);
void from_json(const nlohmann::ordered_json& j, ImageEffectType& effectType);
}