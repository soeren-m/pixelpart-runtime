#pragma once

#include "Types.h"

namespace pixelpart {
enum class ColorMode : uint32_t {
	multiply = 0,
	add = 1,
	subtract = 2,
	difference = 3,
	screen = 4,
	overlay = 5,
	lighten = 6,
	darken = 7,
	coloronly = 8
};
}