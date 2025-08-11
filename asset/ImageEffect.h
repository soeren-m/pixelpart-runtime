#pragma once

#include "../common/VariantParameter.h"
#include "ImageEffectType.h"
#include "../json/json.hpp"
#include <string>
#include <vector>

namespace pixelpart {
class ImageEffect {
public:
	ImageEffect() = default;
	ImageEffect(const ImageEffectType& type);
	ImageEffect(const std::string& typeId, const std::vector<VariantParameter::Value>& parameters, bool visible = true);

	const std::string& type() const;

	std::vector<VariantParameter::Value>& parameters();
	const std::vector<VariantParameter::Value>& parameters() const;

	void visible(bool visible);
	bool visible() const;

private:
	std::string effectTypeId;
	std::vector<VariantParameter::Value> effectParameters;
	bool effectVisible = true;
};

void to_json(nlohmann::ordered_json& j, const ImageEffect& effect);
void from_json(const nlohmann::ordered_json& j, ImageEffect& effect);
}
