#include "EffectInput.h"

namespace pixelpart {
EffectInput::EffectInput(const std::string& inputName, const VariantValue& inputValue) : name(inputName), value(inputValue) {

}

void to_json(nlohmann::ordered_json& j, const EffectInput& input) {
	j = nlohmann::ordered_json{
		{ "name", input.name },
		{ "value", input.value }
	};
}
void from_json(const nlohmann::ordered_json& j, EffectInput& input) {
	input = EffectInput(
		j.value("name", ""),
		j.value("value", VariantValue()));
}
}