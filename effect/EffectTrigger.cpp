#include "EffectTrigger.h"

namespace pixelpart {
EffectTrigger::EffectTrigger(const std::string& name) : triggerName(name) {

}

void EffectTrigger::name(const std::string& name) {
	triggerName = name;
}
const std::string& EffectTrigger::name() const {
	return triggerName;
}

void to_json(nlohmann::ordered_json& j, const EffectTrigger& trigger) {
	j = nlohmann::ordered_json{
		{ "name", trigger.name() }
	};
}
void from_json(const nlohmann::ordered_json& j, EffectTrigger& trigger) {
	trigger = EffectTrigger(j.value("name", ""));
}
}
