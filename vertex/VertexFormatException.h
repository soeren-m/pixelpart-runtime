#pragma once

#include <cstdint>
#include <string>
#include <stdexcept>

namespace pixelpart {
class VertexFormatException : public std::runtime_error {
public:
	VertexFormatException(const char* msg, std::int32_t attribIndex = -1);
	VertexFormatException(const std::string& msg, std::int32_t attribIndex = -1);

	std::int32_t attributeIndex() const;

private:
	std::int32_t exceptionAttributeIndex = -1;
};
}
