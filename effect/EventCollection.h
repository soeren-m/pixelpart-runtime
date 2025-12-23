#pragma once

#include "../common/Id.h"
#include "Event.h"
#include <unordered_map>

namespace pixelpart {
using EventCollection = std::unordered_map<id_t, Event>;
}
