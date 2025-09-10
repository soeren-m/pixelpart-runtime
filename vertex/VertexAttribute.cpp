#include "VertexAttribute.h"

namespace pixelpart {
VertexAttribute::VertexAttribute(VertexAttributeUsage usg, VertexDataGenerationMode genMode, VertexDataType comp,
	std::size_t buffIndex, std::size_t offset, std::size_t stride) :
	usage(usg), dataGenerationMode(genMode), componentType(comp),
	bufferIndex(buffIndex), byteOffset(offset), byteStride(stride) {

}
}
