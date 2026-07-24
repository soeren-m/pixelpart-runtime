#pragma once

#include <cstdint>
#include <limits>

namespace pixelpart {
class pcg32 {
public:
	using result_type = std::uint32_t;

	constexpr pcg32(std::uint64_t initstate = 0x853c49e6748fea9bULL, std::uint64_t initseq = 0xda3e39cb94b95bdbULL) {
		seed(initstate, initseq);
	}

	constexpr result_type operator()() {
		std::uint64_t oldstate = state;
		state = oldstate * 6364136223846793005ULL + increment;

		std::uint32_t xorshifted = static_cast<std::uint32_t>(((oldstate >> 18u) ^ oldstate) >> 27u);
		std::uint32_t rot = static_cast<std::uint32_t>(oldstate >> 59u);

		return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
	}

	constexpr double next() {
		return (operator()() >> 8) * 0x1.0p-24;
	}
	constexpr double next(double min, double max) {
		return (operator()() >> 8) * 0x1.0p-24 * (max - min) + min;
	}

	constexpr void seed(std::uint64_t initstate, std::uint64_t initseq = 0xda3e39cb94b95bdbULL) {
		state = 0;
		increment = (initseq << 1u) | 1u;

		operator()();
		state += initstate;
		operator()();
	}

	static constexpr result_type min() {
		return std::numeric_limits<result_type>::min();
	}

	static constexpr result_type max() {
		return std::numeric_limits<result_type>::max();
	}

private:
	std::uint64_t state = 0;
	std::uint64_t increment = 0;
};
}
