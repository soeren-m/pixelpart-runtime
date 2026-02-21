#pragma once

#include <random>
#include <algorithm>

namespace pixelpart {
namespace math {
template <typename T, typename TRng>
T randomUniform(TRng& rng, T min, T max) {
	return std::uniform_real_distribution<T>(min, max)(rng);
}
template <typename TInt, typename TRng>
TInt randomDiscreteUniform(TRng& rng, TInt min, TInt max) {
	return std::uniform_int_distribution<TInt>(min, max)(rng);
}

template <typename T, typename TRng>
T randomNormal(TRng& rng, T mean, T stddev) {
	return std::normal_distribution<T>(mean, stddev)(rng);
}

template <typename T, typename TRng>
T randomTruncatedNormal(TRng& rng, T min, T max) {
	T mean = (min + max) / T(2);
	T stddev = (max - min) / T(6);
	T value = randomNormal<T, TRng>(rng, mean, stddev);

	return std::clamp(value, min, max);
}
template <typename T, typename TRng>
T randomTruncatedInverseNormal(TRng& rng, T min, T max) {
	T mean = (min + max) / T(2);
	T value = randomTruncatedNormal<T, TRng>(rng, min, max);

	return value > mean
		? mean + (max - value)
		: mean - (value - min);
}

template <typename T, typename TInt, typename TRng>
T randomUniformGrid(TRng& rng, TInt size, T min, T max) {
	return static_cast<T>(randomDiscreteUniform<TInt, TRng>(rng, TInt(0), size - TInt(1))) / static_cast<T>(size - TInt(1)) * (max - min) + min;
}
}
}
