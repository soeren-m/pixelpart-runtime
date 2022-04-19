#pragma once

#include "Types.h"

namespace pixelpart {
vec4d rgb2hsv(const vec4d& colorRGB);
vec4d hsv2rgb(const vec4d& colorHSV);

vec4d hsvAdd(const vec4d& colorRGB, const vec4d& colorHSV, floatd alpha);
}