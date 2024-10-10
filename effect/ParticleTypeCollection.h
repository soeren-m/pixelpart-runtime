#pragma once

#include "../common/Id.h"
#include "ParticleType.h"
#include <unordered_map>

namespace pixelpart {
using ParticleTypeCollection = std::unordered_map<id_t, ParticleType>;
}