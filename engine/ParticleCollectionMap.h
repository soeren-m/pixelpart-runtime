#pragma once

#include "ParticleCollection.h"
#include "../effect/ParticleEmissionPair.h"
#include <unordered_map>

namespace pixelpart {
using ParticleCollectionMap = std::unordered_map<ParticleEmissionPair, ParticleCollection>;
}
