#include "EffectInput.h"

namespace pixelpart {
EffectInput::EffectInput(const std::string& inName, const VariantValue& inValue) : inputName(inName), inputValue(inValue) {

}

void EffectInput::name(const std::string& name) {
	inputName = name;
}
const std::string& EffectInput::name() const {
	return inputName;
}

void EffectInput::value(const VariantValue& value) {
	inputValue = value;
}
const VariantValue& EffectInput::value() const {
	return inputValue;
}

void to_json(nlohmann::ordered_json& j, const EffectInput& input) {
	j = nlohmann::ordered_json{
		{ "name", input.name() },
		{ "value", input.value() }
	};
}
void from_json(const nlohmann::ordered_json& j, EffectInput& input) {
	input = EffectInput(
		j.value("name", ""),
		j.value("value", VariantValue()));
}
}
