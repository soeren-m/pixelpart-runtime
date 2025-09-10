#pragma once

#include <cstdint>

namespace pixelpart {
enum class VertexDataType : std::uint32_t {
	type_uint8,
	type_uint16,
	type_uint32,
	type_int8,
	type_int16,
	type_int32,
	type_float
};
}
