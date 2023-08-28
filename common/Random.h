#pragma once

#include "Types.h"
#include <random>

namespace pixelpart {
namespace random {
template <typename T>
floatd uniform(T& rng, floatd min, floatd max) {
	return std::uniform_real_distribution<floatd>(min, max)(rng);
}

template <typename T>
floatd uniformInt(T& rng, int32_t min, int32_t max) {
	return std::uniform_int_distribution<int32_t>(min, max)(rng);
}

template <typename T>
floatd normal(T& rng, floatd min, floatd max) {
	floatd m = (min + max) / 2.0;
	floatd d = (max - min) / 6.0;
	std::normal_distribution<floatd> distrib(m, d);

	floatd x = distrib(rng);
	while(x < min || x > max) {
		x = distrib(rng);
	}

	return x;
}

template <typename T>
floatd normalReverse(T& rng, floatd min, floatd max) {
	floatd m = (min + max) / 2.0;
	floatd x = normal(rng, min, max);

	return (x > m)
		? m + (max - x)
		: m - (x - min);
}
}
}