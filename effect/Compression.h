#pragma once

#include "CompressionMethod.h"
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

std::string compressToString(const unsigned char* data, std::size_t size, CompressionMethod method);
std::vector<unsigned char> decompressFromString(const std::string& data, std::size_t uncompressedSize, CompressionMethod method);
}
