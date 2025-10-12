#pragma once

#include <cstddef>

namespace pixelpart {
template <typename T>
void combineHash(std::size_t& seed, const T& v) {
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
}
