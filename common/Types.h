#pragma once

#include <cstdint>
#include <cstddef>

namespace pixelpart {
using int_t = std::int64_t;
using float_t = double;

pixelpart::int_t* valuePtr(pixelpart::int_t& v);
pixelpart::float_t* valuePtr(pixelpart::float_t& v);

const pixelpart::int_t* valuePtr(const pixelpart::int_t& v);
const pixelpart::float_t* valuePtr(const pixelpart::float_t& v);
}
