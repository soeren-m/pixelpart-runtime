#pragma once

#include "VertexAttributeUsage.h"
#include "VertexDataGenerationMode.h"
#include "VertexDataType.h"
#include <cstddef>

namespace pixelpart {
struct VertexAttribute {
	VertexAttributeUsage usage = VertexAttributeUsage::position3d;
	VertexDataGenerationMode dataGenerationMode = VertexDataGenerationMode::vertex;
	VertexDataType componentType = VertexDataType::type_float;
	std::size_t bufferIndex = 0;
	std::size_t byteOffset = 0;
	std::size_t byteStride = 0;

	VertexAttribute() = default;
	VertexAttribute(VertexAttributeUsage usg, VertexDataGenerationMode genMode, VertexDataType comp,
		std::size_t buffIndex, std::size_t offset, std::size_t stride);
};
}
