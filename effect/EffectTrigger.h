#pragma once

#include "../json/json.hpp"
#include <string>

namespace pixelpart {
class EffectTrigger {
public:
	EffectTrigger() = default;
	EffectTrigger(const std::string& name);

	void name(const std::string& name);
	const std::string& name() const;

private:
	std::string triggerName;
};

void to_json(nlohmann::ordered_json& j, const EffectTrigger& trigger);
void from_json(const nlohmann::ordered_json& j, EffectTrigger& trigger);
}
