#pragma once

#include "VertexFormat.h"
#include "VertexAttribute.h"
#include "VertexAttributeUsage.h"
#include "VertexDataGenerationMode.h"
#include "VertexDataType.h"
#include "VertexWindingOrder.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace pixelpart {
class VertexFormatBuilder {
public:
	VertexFormatBuilder() = default;

	VertexFormat build() const;

	void addAttribute(VertexAttributeUsage usage, VertexDataGenerationMode dataGenerationMode, VertexDataType componentType,
		std::size_t bufferIndex, std::size_t byteOffset, std::size_t byteStride);

	void windingOrder(VertexWindingOrder winding);

private:
	std::vector<VertexAttribute> formatAttributes;
	VertexWindingOrder formatWindingOrder = VertexWindingOrder::cw;
};
}
