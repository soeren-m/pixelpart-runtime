#pragma once

#include "EffectInput.h"
#include <unordered_map>

namespace pixelpart {
using EffectInputCollection = std::unordered_map<id_t, EffectInput>;
}