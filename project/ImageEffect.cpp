#include "ImageEffect.h"
#include "../common/Json.h"

namespace pixelpart {
ImageEffect::ImageEffect() {

}
ImageEffect::ImageEffect(const ImageEffectType& effectType) : type(effectType.name), parameters(effectType.parameters.size()) {
	for(std::size_t p = 0u; p < parameters.size(); p++) {
		parameters[p] = effectType.parameters[p].defaultValue;
	}
}

void to_json(nlohmann::ordered_json& j, const ImageEffect& effect) {
	j = nlohmann::ordered_json{
		{ "type", effect.type },
		{ "parameters", effect.parameters },
		{ "visible", effect.visible }
	};
}
void from_json(const nlohmann::ordered_json& j, ImageEffect& effect) {
	effect = ImageEffect();

	fromJson(effect.type, j, "type");
	fromJson(effect.parameters, j, "parameters");
	fromJson(effect.visible, j, "visible");
}
}