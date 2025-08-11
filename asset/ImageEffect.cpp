#include "ImageEffect.h"
#include <cstddef>

namespace pixelpart {
ImageEffect::ImageEffect(const ImageEffectType& type) :
	effectTypeId(type.name), effectParameters(type.parameters.size()) {
	for(std::size_t parameterIndex = 0; parameterIndex < effectParameters.size(); parameterIndex++) {
		effectParameters[parameterIndex] = type.parameters[parameterIndex].def();
	}
}
ImageEffect::ImageEffect(const std::string& typeId, const std::vector<VariantParameter::Value>& parameters, bool visible) :
	effectTypeId(typeId), effectParameters(parameters), effectVisible(visible) {

}

const std::string& ImageEffect::type() const {
	return effectTypeId;
}

std::vector<VariantParameter::Value>& ImageEffect::parameters() {
	return effectParameters;
}
const std::vector<VariantParameter::Value>& ImageEffect::parameters() const {
	return effectParameters;
}

void ImageEffect::visible(bool visible) {
	effectVisible = visible;
}
bool ImageEffect::visible() const {
	return effectVisible;
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
