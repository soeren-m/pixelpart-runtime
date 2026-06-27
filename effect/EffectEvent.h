#pragma once

#include "../types/Types.h"
#include "../types/Id.h"
#include "../json/json.hpp"
#include <string>

namespace pixelpart {
class EffectEvent {
public:
	EffectEvent() = default;
	EffectEvent(const std::string& name, float_t time);
	EffectEvent(const std::string& name, id_t nodeId, float_t time);

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

void to_json(nlohmann::ordered_json& j, const EffectEvent& event);
void from_json(const nlohmann::ordered_json& j, EffectEvent& event);
}
