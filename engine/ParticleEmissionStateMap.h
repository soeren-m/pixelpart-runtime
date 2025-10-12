#pragma once

#include "ParticleEmissionState.h"
#include "../effect/ParticleEmissionPair.h"
#include <unordered_map>

namespace pixelpart {
using ParticleEmissionStateMap = std::unordered_map<ParticleEmissionPair, ParticleEmissionState>;
}
