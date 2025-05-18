#pragma once

#include "CompressionMethod.h"
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

namespace pixelpart {
std::string compressBase64(const uint8_t* data, std::size_t size, CompressionMethod method);
std::vector<uint8_t> decompressBase64(const std::string& data, std::size_t uncompressedSize, CompressionMethod method);
}