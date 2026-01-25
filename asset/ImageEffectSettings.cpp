#include "ImageEffectSettings.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const ImageEffectSettings& settings) {
	j = nlohmann::ordered_json{
		{ "effects", settings.effects }
	};
}
void from_json(const nlohmann::ordered_json& j, ImageEffectSettings& settings) {
	settings = ImageEffectSettings();
	settings.effects = j.value("effects", std::vector<ImageEffect>());
}
}
