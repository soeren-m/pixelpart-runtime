#pragma once

#include "../common/Id.h"
#include "EffectEvent.h"
#include <unordered_map>

namespace pixelpart {
using EffectEventCollection = std::unordered_map<id_t, EffectEvent>;
}
