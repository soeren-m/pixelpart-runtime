#include "VertexFormatException.h"

namespace pixelpart {
VertexFormatException::VertexFormatException(const char* msg, std::int32_t attribIndex) :
	std::runtime_error(msg), exceptionAttributeIndex(attribIndex) {

}
VertexFormatException::VertexFormatException(const std::string& msg, std::int32_t attribIndex) :
	std::runtime_error(msg), exceptionAttributeIndex(attribIndex) {

}

std::int32_t VertexFormatException::attributeIndex() const {
	return exceptionAttributeIndex;
}
}
