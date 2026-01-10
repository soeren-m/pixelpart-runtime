#pragma once

#include "CompressionMethod.h"
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

namespace pixelpart {
std::string compressToString(const std::uint8_t* data, std::size_t size, CompressionMethod method);
std::vector<std::uint8_t> decompressFromString(const std::string& data, std::size_t uncompressedSize, CompressionMethod method);
}
