#include "ImageEffect.h"
#include <cstddef>

namespace pixelpart {
ImageEffect::ImageEffect(const ImageEffectType& type) :
	effectTypeId(type.name), effectParameters(type.parameters.size()) {
	for(std::size_t p = 0u; p < effectParameters.size(); p++) {
		effectParameters[p] = type.parameters[p].def();
	}
}
ImageEffect::ImageEffect(const std::string& typeId, const std::vector<VariantParameter::Value>& parameters, bool visible) :
	effectTypeId(typeId), effectParameters(parameters), effectIsVisible(visible) {

}

void to_json(nlohmann::ordered_json& j, const ImageEffect& effect) {
	j = nlohmann::ordered_json{
		{ "type", effect.type() },
		{ "parameters", effect.parameters() },
		{ "visible", effect.visible() }
	};
}
void from_json(const nlohmann::ordered_json& j, ImageEffect& effect) {
	effect = ImageEffect(
		j.at("type"),
		j.value("parameters", std::vector<VariantParameter::Value>()),
		j.value("visible", true));
}
}