#include "VertexFormatBuilder.h"

namespace pixelpart {
VertexFormat VertexFormatBuilder::build() const {
	return VertexFormat(formatAttributes, formatWindingOrder);
}

void VertexFormatBuilder::addAttribute(VertexAttributeUsage usage, VertexDataGenerationMode dataGenerationMode, VertexDataType componentType,
	std::size_t bufferIndex, std::size_t byteOffset, std::size_t byteStride) {
	formatAttributes.emplace_back(
		usage, dataGenerationMode, componentType,
		bufferIndex, byteOffset, byteStride);
}

void VertexFormatBuilder::windingOrder(VertexWindingOrder winding) {
	formatWindingOrder = winding;
}
}
