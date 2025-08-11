#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

namespace pixelpart {
std::string encodeBase64(const std::uint8_t* data, std::size_t size);
std::string decodeBase64(const std::string& data);
}
