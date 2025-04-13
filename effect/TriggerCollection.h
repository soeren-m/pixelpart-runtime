#pragma once

#include "../common/Id.h"
#include "Trigger.h"
#include <unordered_map>

namespace pixelpart {
using TriggerCollection = std::unordered_map<id_t, Trigger>;
}