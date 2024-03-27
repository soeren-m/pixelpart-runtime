#pragma once

#include <string>

namespace pixelpart {
namespace base64 {
std::string encode(const unsigned char* data, std::size_t size);
std::string decode(const std::string& data);
}
}