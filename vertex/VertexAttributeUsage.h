#pragma once

#include <cstdint>

namespace pixelpart {
enum class VertexAttributeUsage : std::uint32_t {
	index,
	position2d,
	position3d,
	normal,
	texture_coord,
	color,
	life,
	id,
	velocity2d,
	velocity3d,
	matrix4x4_column_major,
	matrix4x3_column_major,
	matrix4x4_row_major,
	matrix4x3_row_major
};
}
