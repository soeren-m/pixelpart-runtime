#pragma once

#include "Types.h"

namespace pixelpart {
struct Node {
	std::string name;
	floatd lifetimeStart = 0.0;
	floatd lifetimeDuration = 1.0;
	bool repeat = true;
};
}