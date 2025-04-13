#pragma once

#include "../common/VariantValue.h"
#include "../json/json.hpp"
#include <string>

namespace pixelpart {
class EffectInput {
public:
	EffectInput() = default;
	EffectInput(const std::string& inName, const VariantValue& inValue);

	void name(const std::string& name);
	const std::string& name() const;

	void value(const VariantValue& value);
	const VariantValue& value() const;

private:
	std::string inputName;
	VariantValue inputValue;
};

void to_json(nlohmann::ordered_json& j, const EffectInput& input);
void from_json(const nlohmann::ordered_json& j, EffectInput& input);
}