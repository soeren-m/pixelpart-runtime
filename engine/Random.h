#pragma once

#include "../common/Types.h"
#include <random>

namespace pixelpart {
namespace random {
template <typename T>
float_t uniform(T& rng, float_t min, float_t max) {
	return std::uniform_real_distribution<float_t>(min, max)(rng);
}

template <typename T>
float_t uniformInt(T& rng, std::int32_t min, std::int32_t max) {
	return std::uniform_int_distribution<std::int32_t>(min, max)(rng);
}

template <typename T>
float_t normal(T& rng, float_t min, float_t max) {
	float_t m = (min + max) / 2.0;
	float_t d = (max - min) / 6.0;
	std::normal_distribution<float_t> distrib(m, d);

	float_t x = distrib(rng);
	while(x < min || x > max) {
		x = distrib(rng);
	}

	return x;
}

template <typename T>
float_t normalReverse(T& rng, float_t min, float_t max) {
	float_t m = (min + max) / 2.0;
	float_t x = normal(rng, min, max);

	return (x > m)
		? m + (max - x)
		: m - (x - min);
}

template <typename T>
float_t uniformGrid(T& rng, std::uint32_t size) {
	return static_cast<float_t>(uniformInt(rng, 0, size - 1)) / static_cast<float_t>(size - 1);
}

template <typename T>
float_t uniformGrid(T& rng, std::uint32_t size, float_t min, float_t max) {
	return static_cast<float_t>(uniformInt(rng, 0, size - 1)) / static_cast<float_t>(size - 1) * (max - min) + min;
}
}
}