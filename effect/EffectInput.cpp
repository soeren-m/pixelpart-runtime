#include "EffectInput.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const EffectInput& input) {
	j = nlohmann::ordered_json{
		{ "name", input.name },
		{ "value", input.value }
	};
}
void from_json(const nlohmann::ordered_json& j, EffectInput& input) {
	input = EffectInput();
	input.name = j.at("name");
	input.value = j.at("value");
}
}