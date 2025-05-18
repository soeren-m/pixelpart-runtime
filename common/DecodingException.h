#pragma once

#include <string>
#include <stdexcept>

namespace pixelpart {
class DecodingException : public std::runtime_error {
public:
	DecodingException(const char* msg);
	DecodingException(const std::string& msg);
};
}