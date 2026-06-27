#include "Compression.h"
#include "Encoding.h"
#include "../zlib/zlib.h"

namespace pixelpart {
CompressionException::CompressionException(const char* msg) : std::runtime_error(msg) {

}
CompressionException::CompressionException(const std::string& msg) : std::runtime_error(msg) {

}

DecompressionException::DecompressionException(const char* msg) : std::runtime_error(msg) {

}
DecompressionException::DecompressionException(const std::string& msg) : std::runtime_error(msg) {

}

std::string compressToString(const std::uint8_t* data, std::size_t size, CompressionMethod method) {
	if(!data || size == 0) {
		throw CompressionException("No data available");
	}

	if(method == CompressionMethod::zlib) {
		uLong compressedSize = compressBound(static_cast<uLong>(size));
		std::vector<Byte> compressedData(compressedSize);

		int result = compress(
			compressedData.data(),
			&compressedSize,
			data,
			static_cast<uLong>(size));

		if(result != Z_OK) {
			throw CompressionException("zlib compression error: " + std::to_string(result));
		}

		return encodeBase64(static_cast<const std::uint8_t*>(compressedData.data()), static_cast<std::size_t>(compressedSize));
	}

	return encodeBase64(data, size);
}

std::vector<std::uint8_t> decompressFromString(const std::string& data, std::size_t uncompressedSize, CompressionMethod method) {
	std::string compressedData = decodeBase64(data);
	if(compressedData.empty()) {
		throw DecompressionException("Compressed data is empty");
	}

	if(method == CompressionMethod::zlib) {
		std::vector<std::uint8_t> uncompressedData(uncompressedSize);

		uLong uncompressedSizeValue = static_cast<uLong>(uncompressedSize);
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

	return std::vector<std::uint8_t>(compressedData.begin(), compressedData.end());
}
}
