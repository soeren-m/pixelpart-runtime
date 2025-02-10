#pragma once

#include "../common/Types.h"

namespace pixelpart {
namespace noise {
float_t simplex(float_t x);
float_t simplex(float_t x, float_t y);
float_t simplex(float_t x, float_t y, float_t z);
float_t simplex(float_t x, float_t y, float_t z, float_t w);

float_t simplexFBM(uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity, float_t x);
float_t simplexFBM(uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity, float_t x, float_t y);
float_t simplexFBM(uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity, float_t x, float_t y, float_t z);
float_t simplexFBM(uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity, float_t x, float_t y, float_t z, float_t w);
}
}