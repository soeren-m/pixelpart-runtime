#pragma once

#include "EffectTrigger.h"
#include "../types/Id.h"
#include <unordered_map>

namespace pixelpart {
using EffectTriggerCollection = std::unordered_map<id_t, EffectTrigger>;
}
