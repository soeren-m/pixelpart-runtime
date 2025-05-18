#pragma once

#include <string>
#include <stdexcept>

namespace pixelpart {
class DeserializationException : public std::runtime_error {
public:
	DeserializationException(const char* msg);
	DeserializationException(const std::string& msg);
};
}