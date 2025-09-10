#pragma once

#include <cstdint>

namespace pixelpart {
enum class VertexDataGenerationMode : std::uint32_t {
	vertex,
	element,
	instance
};
}
