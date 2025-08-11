#include "RuntimeContext.h"

namespace pixelpart {
RuntimeContext::RuntimeContext(float_t t, float_t dt) :
	contextTime(t), contextDeltaTime(dt) {

}
RuntimeContext::RuntimeContext(float_t t, float_t dt, const TriggerActivationTimeMap& triggerActivationTimes) :
	contextTime(t), contextDeltaTime(dt), contextTriggerActivationTimes(triggerActivationTimes) {

}

float_t RuntimeContext::time() const {
	return contextTime;
}
float_t RuntimeContext::deltaTime() const {
	return contextDeltaTime;
}

bool RuntimeContext::triggerActivated(id_t triggerId) const {
	return contextTriggerActivationTimes.count(triggerId) != 0;
}
float_t RuntimeContext::triggerActivationTime(id_t triggerId) const {
	auto triggerIt = contextTriggerActivationTimes.find(triggerId);

	return triggerIt != contextTriggerActivationTimes.end() ? triggerIt->second : -1.0;
}
const RuntimeContext::TriggerActivationTimeMap& RuntimeContext::triggerActivationTimes() const {
	return contextTriggerActivationTimes;
}
}
