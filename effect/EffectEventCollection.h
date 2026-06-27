#pragma once

#include "EffectEvent.h"
#include "../types/Id.h"
#include <unordered_map>

namespace pixelpart {
using EffectEventCollection = std::unordered_map<id_t, EffectEvent>;
}
