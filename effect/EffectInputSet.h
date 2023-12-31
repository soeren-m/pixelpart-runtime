#pragma once

#include "EffectInput.h"
#include <unordered_map>

namespace pixelpart {
using EffectInputSet = std::unordered_map<uint32_t, EffectInput>;
}