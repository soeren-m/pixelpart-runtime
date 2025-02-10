#pragma once

#include "../json/json.hpp"
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

namespace pixelpart {
enum class CompressionMethod : uint32_t {
	none = 0,
	zlib = 1
};

std::string compressAndEncode(const uint8_t* data, std::size_t size, CompressionMethod method);
std::vector<uint8_t> decodeAndDecompress(const std::string& data, std::size_t uncompressedSize, CompressionMethod method);

NLOHMANN_JSON_SERIALIZE_ENUM(CompressionMethod, {
	{ CompressionMethod::none, "none" },
	{ CompressionMethod::zlib, "zlib" }
})
}