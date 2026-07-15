#pragma once

#include "../types/Types.h"
#include "../types/Id.h"
#include <cstdint>
#include <vector>
#include <unordered_map>

namespace pixelpart {
class EffectRuntimeContext {
public:
	using TriggerActivationTimeMap = std::unordered_map<id_t, float_t>;
	using EventInvokationList = std::vector<id_t>;

	EffectRuntimeContext() = default;
	EffectRuntimeContext(float_t t);

	float_t& time();
	float_t time() const;

	float_t& deltaTime();
	float_t deltaTime() const;

	std::uint32_t& lod();
	std::uint32_t lod() const;

	TriggerActivationTimeMap& triggerActivationTimes();
	const TriggerActivationTimeMap& triggerActivationTimes() const;
	bool triggerActivated(id_t triggerId) const;
	float_t triggerActivationTime(id_t triggerId) const;

	EventInvokationList& invokedEvents();
	const EventInvokationList& invokedEvents() const;

private:
	float_t contextTime = 0.0;
	float_t contextDeltaTime = 0.0;
	std::uint32_t contextLod = 0;
	TriggerActivationTimeMap contextTriggerActivationTimes;
	EventInvokationList contextInvokedEvents;
};
}
