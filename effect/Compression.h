#pragma once

#include "CompressionMethod.h"
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <stdexcept>

namespace pixelpart {
class CompressionException : public std::runtime_error {
public:
	CompressionException(const char* msg);
	CompressionException(const std::string& msg);
};

class DecompressionException : public std::runtime_error {
public:
	DecompressionException(const char* msg);
	DecompressionException(const std::string& msg);
};

std::string compressToString(const std::uint8_t* data, std::size_t size, CompressionMethod method);
std::vector<std::uint8_t> decompressFromString(const std::string& data, std::size_t uncompressedSize, CompressionMethod method);
}
