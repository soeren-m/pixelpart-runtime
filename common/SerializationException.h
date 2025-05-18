#pragma once

#include <string>
#include <stdexcept>

namespace pixelpart {
class SerializationException : public std::runtime_error {
public:
	SerializationException(const char* msg);
	SerializationException(const std::string& msg);
};
}