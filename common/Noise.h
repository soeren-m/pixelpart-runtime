#pragma once

#include "Types.h"

namespace pixelpart {
namespace noise {
floatd simplexNoise(floatd x);
floatd simplexNoise(floatd x, floatd y);
floatd simplexNoise(floatd x, floatd y, floatd z);
floatd simplexNoise(floatd x, floatd y, floatd z, floatd w);

floatd fBmSimplexNoise(uint32_t octaves, floatd frequency, floatd persistence, floatd lacunarity, floatd x);
floatd fBmSimplexNoise(uint32_t octaves, floatd frequency, floatd persistence, floatd lacunarity, floatd x, floatd y);
floatd fBmSimplexNoise(uint32_t octaves, floatd frequency, floatd persistence, floatd lacunarity, floatd x, floatd y, floatd z);
floatd fBmSimplexNoise(uint32_t octaves, floatd frequency, floatd persistence, floatd lacunarity, floatd x, floatd y, floatd z, floatd w);
}
}