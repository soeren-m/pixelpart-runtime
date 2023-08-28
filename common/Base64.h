#pragma once

#include <string>

namespace pixelpart {
std::string encodeBase64(const unsigned char* data, std::size_t size);
std::string decodeBase64(const std::string& data);
}