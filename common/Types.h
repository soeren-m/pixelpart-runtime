#pragma once

#include <cstdint>
#include <cstddef>

namespace pixelpart {
using int_t = std::int64_t;
using float_t = double;

int_t* valuePtr(int_t& v);
float_t* valuePtr(float_t& v);

const int_t* valuePtr(const int_t& v);
const float_t* valuePtr(const float_t& v);
}
