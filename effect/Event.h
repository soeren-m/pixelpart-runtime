#pragma once

#include "../common/Types.h"
#include "../common/Id.h"
#include "../json/json.hpp"
#include <string>

namespace pixelpart {
class Event {
public:
	Event(const std::string& name, float_t time);
	Event(const std::string& name, id_t nodeId, float_t time);

	void name(const std::string& name);
	const std::string& name() const;

	void nodeId(id_t nodeId);
	id_t nodeId() const;

	void time(float_t time);
	float_t time() const;

private:
	std::string eventName;
	id_t eventNodeId;
	float_t eventTime = 0.0;
};

void to_json(nlohmann::ordered_json& j, const Event& event);
void from_json(const nlohmann::ordered_json& j, Event& event);
}
