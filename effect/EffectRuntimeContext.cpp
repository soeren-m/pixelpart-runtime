#include "EffectRuntimeContext.h"

namespace pixelpart {
EffectRuntimeContext::EffectRuntimeContext(float_t t, float_t dt) :
	contextTime(t), contextDeltaTime(dt) {

}
EffectRuntimeContext::EffectRuntimeContext(float_t t, float_t dt, const TriggerActivationTimeMap& triggerActivationTimes) :
	contextTime(t), contextDeltaTime(dt), contextTriggerActivationTimes(triggerActivationTimes) {

}

float_t& EffectRuntimeContext::time() {
	return contextTime;
}
float_t EffectRuntimeContext::time() const {
	return contextTime;
}

float_t& EffectRuntimeContext::deltaTime() {
	return contextDeltaTime;
}
float_t EffectRuntimeContext::deltaTime() const {
	return contextDeltaTime;
}

EffectRuntimeContext::TriggerActivationTimeMap& EffectRuntimeContext::triggerActivationTimes() {
	return contextTriggerActivationTimes;
}
const EffectRuntimeContext::TriggerActivationTimeMap& EffectRuntimeContext::triggerActivationTimes() const {
	return contextTriggerActivationTimes;
}

bool EffectRuntimeContext::triggerActivated(id_t triggerId) const {
	return contextTriggerActivationTimes.count(triggerId) != 0;
}
float_t EffectRuntimeContext::triggerActivationTime(id_t triggerId) const {
	auto triggerIt = contextTriggerActivationTimes.find(triggerId);

	return triggerIt != contextTriggerActivationTimes.end() ? triggerIt->second : -1.0;
}
}
