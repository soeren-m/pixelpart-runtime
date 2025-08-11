#include "Trigger.h"

namespace pixelpart {
Trigger::Trigger(const std::string& name) : triggerName(name) {

}

void Trigger::name(const std::string& name) {
	triggerName = name;
}
const std::string& Trigger::name() const {
	return triggerName;
}

void to_json(nlohmann::ordered_json& j, const Trigger& trigger) {
	j = nlohmann::ordered_json{
		{ "name", trigger.name() }
	};
}
void from_json(const nlohmann::ordered_json& j, Trigger& trigger) {
	trigger = Trigger(j.value("name", ""));
}
}
