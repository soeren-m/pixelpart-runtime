#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <stdexcept>

namespace pixelpart {
class DecodingException : public std::runtime_error {
public:
	DecodingException(const char* msg);
	DecodingException(const std::string& msg);
};

std::string encodeBase64(const std::uint8_t* data, std::size_t size);
std::string decodeBase64(const std::string& data);
}
