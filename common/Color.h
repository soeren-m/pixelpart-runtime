#pragma once

#include "Types.h"

namespace pixelpart {
vec4_t rgb2hsv(const vec4_t& colorRGB);
vec4_t hsv2rgb(const vec4_t& colorHSV);

vec4_t hsvAdd(const vec4_t& colorRGB, const vec4_t& colorHSV, float_t alpha);
}