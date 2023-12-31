#pragma once

#include "../common/VariantValue.h"

namespace pixelpart {
struct EffectInput {
	std::string name;
	VariantValue value;
};

void to_json(nlohmann::ordered_json& j, const EffectInput& input);
void from_json(const nlohmann::ordered_json& j, EffectInput& input);
}