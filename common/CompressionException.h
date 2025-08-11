#pragma once

#include <string>
#include <stdexcept>

namespace pixelpart {
class CompressionException : public std::runtime_error {
public:
	CompressionException(const char* msg);
	CompressionException(const std::string& msg);
};
}
