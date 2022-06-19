#pragma once

#include "Types.h"

namespace pixelpart {
struct Node {
	uint32_t id = NullId;
	uint32_t parentId = NullId;
	std::string name;

	floatd lifetimeStart = 0.0;
	floatd lifetimeDuration = 1.0;
	bool repeat = true;
};
}