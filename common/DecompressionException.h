#pragma once

#include <string>
#include <stdexcept>

namespace pixelpart {
class DecompressionException : public std::runtime_error {
public:
	DecompressionException(const char* msg);
	DecompressionException(const std::string& msg);
};
}
