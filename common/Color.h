#pragma once

#include "Math.h"

namespace pixelpart {
vec4_t rgb2hsv(const vec4_t& colorRGB);
vec4_t hsv2rgb(const vec4_t& colorHSV);
}