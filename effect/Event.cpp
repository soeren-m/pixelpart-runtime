#include "Event.h"

namespace pixelpart {
Event::Event(const std::string& name, float_t time) :
	eventName(name), eventTime(time) {

}
Event::Event(const std::string& name, id_t nodeId, float_t time) :
	eventName(name), eventNodeId(nodeId), eventTime(time) {

}

void Event::name(const std::string& name) {
	eventName = name;
}
const std::string& Event::name() const {
	return eventName;
}

void Event::nodeId(id_t nodeId) {
	eventNodeId = nodeId;
}
id_t Event::nodeId() const {
	return eventNodeId;
}

void Event::time(float_t time) {
	eventTime = time;
}
float_t Event::time() const {
	return eventTime;
}

void to_json(nlohmann::ordered_json& j, const Event& event) {
	j = nlohmann::ordered_json{
		{ "name", event.name() },
		{ "node_id", event.nodeId() },
		{ "time", event.time() }
	};
}
void from_json(const nlohmann::ordered_json& j, Event& event) {
	event = Event(
		j.value("name", ""),
		j.value("node_id", id_t()),
		j.value("time", 0.0));
}
}
