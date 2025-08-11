#pragma once

#include "Math.h"

namespace pixelpart {
float4_t rgb2hsv(const float4_t& colorRGB);
float4_t hsv2rgb(const float4_t& colorHSV);
}
