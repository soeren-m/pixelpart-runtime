#include "EffectEvent.h"

namespace pixelpart {
EffectEvent::EffectEvent(const std::string& name, float_t time) :
	eventName(name), eventTime(time) {

}
EffectEvent::EffectEvent(const std::string& name, id_t nodeId, float_t time) :
	eventName(name), eventNodeId(nodeId), eventTime(time) {

}

void EffectEvent::name(const std::string& name) {
	eventName = name;
}
const std::string& EffectEvent::name() const {
	return eventName;
}

void EffectEvent::nodeId(id_t nodeId) {
	eventNodeId = nodeId;
}
id_t EffectEvent::nodeId() const {
	return eventNodeId;
}

void EffectEvent::time(float_t time) {
	eventTime = time;
}
float_t EffectEvent::time() const {
	return eventTime;
}

void to_json(nlohmann::ordered_json& j, const EffectEvent& event) {
	j = nlohmann::ordered_json{
		{ "name", event.name() },
		{ "node_id", event.nodeId() },
		{ "time", event.time() }
	};
}
void from_json(const nlohmann::ordered_json& j, EffectEvent& event) {
	event = EffectEvent(
		j.value("name", ""),
		j.value("node_id", id_t()),
		j.value("time", 0.0));
}
}
