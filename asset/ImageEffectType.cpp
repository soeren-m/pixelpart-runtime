#include "ImageEffectType.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const ImageEffectType& effectType) {
	j = nlohmann::ordered_json{
		{ "name", effectType.name },
		{ "category", effectType.category },
		{ "code", effectType.code },
		{ "parameters", effectType.parameters }
	};
}
void from_json(const nlohmann::ordered_json& j, ImageEffectType& effectType) {
	effectType = ImageEffectType();
	effectType.name = j.at("name");
	effectType.category = j.value("category", "");
	effectType.code = j.value("code", "");
	effectType.parameters = j.value("parameters", std::vector<VariantParameter>());
}
}