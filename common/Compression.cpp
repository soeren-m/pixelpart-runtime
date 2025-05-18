#include "Compression.h"
#include "CompressionException.h"
#include "DecompressionException.h"
#include "Base64.h"
#include "../zlib/zlib.h"

namespace pixelpart {
std::string compressBase64(const uint8_t* data, std::size_t size, CompressionMethod method) {
	if(size == 0u) {
		throw CompressionException("No data available");
	}

	if(method == CompressionMethod::zlib) {
		uLong compressedSize = compressBound(static_cast<uLong>(size));
		std::vector<Byte> compressedData(compressedSize);

		int result = compress(
			compressedData.data(),
			&compressedSize,
			data,
			size);

		if(result != Z_OK) {
			throw CompressionException("zlib compression error: " + std::to_string(result));
		}

		return encodeBase64(static_cast<const uint8_t*>(compressedData.data()), static_cast<std::size_t>(compressedSize));
	}

	return encodeBase64(data, size);
}

std::vector<uint8_t> decompressBase64(const std::string& data, std::size_t uncompressedSize, CompressionMethod method) {
	std::string compressedData = decodeBase64(data);
	if(compressedData.empty()) {
		throw DecompressionException("Compressed data is empty");
	}

	if(method == CompressionMethod::zlib) {
		std::vector<uint8_t> uncompressedData(uncompressedSize);

		uLongf uncompressedSizeValue = static_cast<uLongf>(uncompressedSize);
		int result = uncompress(
			uncompressedData.data(),
			&uncompressedSizeValue,
			reinterpret_cast<const Bytef*>(compressedData.data()),
			static_cast<uLong>(compressedData.size()));

		if(result != Z_OK) {
			throw DecompressionException("zlib decompression error: " + std::to_string(result));
		}

		return uncompressedData;
	}

	return std::vector<uint8_t>(compressedData.begin(), compressedData.end());
}
}