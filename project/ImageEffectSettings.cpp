#include "ImageEffectSettings.h"
#include "../common/Json.h"

namespace pixelpart {
ImageEffectCollection ImageEffectSettings::effectCollection = ImageEffectCollection();

void to_json(nlohmann::ordered_json& j, const ImageEffectSettings& settings) {
	j = nlohmann::ordered_json{
		{ "effects", settings.effects }
	};
}
void from_json(const nlohmann::ordered_json& j, ImageEffectSettings& settings) {
	settings = ImageEffectSettings();

	fromJson(settings.effects, j, "effects");
}
}