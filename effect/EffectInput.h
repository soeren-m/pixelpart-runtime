#pragma once

#include "../common/VariantValue.h"
#include "../json/json.hpp"
#include <string>

namespace pixelpart {
struct EffectInput {
	EffectInput() = default;
	EffectInput(const std::string& inputName, const VariantValue& inputValue);

	std::string name;
	VariantValue value;
};

void to_json(nlohmann::ordered_json& j, const EffectInput& input);
void from_json(const nlohmann::ordered_json& j, EffectInput& input);
}