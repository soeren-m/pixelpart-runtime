#pragma once

#include "Types.h"

namespace pixelpart {
enum class TextureOrigin : uint32_t {
	bottom_left = 0,
	bottom_right = 1,
	top_left = 2,
	top_right = 3
};
}