#include "RuntimeContext.h"

namespace pixelpart {
RuntimeContext::RuntimeContext(float_t t, float_t dt) :
	contextCurrentTime(t), contextDeltaTime(dt) {

}
RuntimeContext::RuntimeContext(float_t t, float_t dt, const TriggerActivationTimeMap& triggerActivationTimes) :
	contextCurrentTime(t), contextDeltaTime(dt), contextTriggerActivationTimes(triggerActivationTimes) {

}

float_t RuntimeContext::currentTime() const {
	return contextCurrentTime;
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