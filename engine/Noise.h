#pragma once

#include "../common/Types.h"

namespace pixelpart {
namespace noise {
float_t simplexNoise(float_t x);
float_t simplexNoise(float_t x, float_t y);
float_t simplexNoise(float_t x, float_t y, float_t z);
float_t simplexNoise(float_t x, float_t y, float_t z, float_t w);

float_t fBmSimplexNoise(uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity, float_t x);
float_t fBmSimplexNoise(uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity, float_t x, float_t y);
float_t fBmSimplexNoise(uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity, float_t x, float_t y, float_t z);
float_t fBmSimplexNoise(uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity, float_t x, float_t y, float_t z, float_t w);
}
}