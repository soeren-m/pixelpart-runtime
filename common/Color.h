#pragma once

#include "Types.h"

namespace pixelpart {
float4_t rgb2hsv(const float4_t& rgba);
float4_t hsv2rgb(const float4_t& hsva);
}
