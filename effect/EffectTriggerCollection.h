#pragma once

#include "../common/Id.h"
#include "EffectTrigger.h"
#include <unordered_map>

namespace pixelpart {
using EffectTriggerCollection = std::unordered_map<id_t, EffectTrigger>;
}
