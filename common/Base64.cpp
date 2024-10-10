#include "Base64.h"
#include <stdexcept>

namespace pixelpart {
std::string encodeBase64(const unsigned char* data, std::size_t size) {
	static const char* base64Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	std::string result;
	result.reserve((size + 2u) / 3u * 4u);

	for(std::size_t i = 0u; i < size; i += 3u) {
		result += base64Table[(data[i] & 0xfc) >> 2];

		if(i + 1u < size) {
			result += base64Table[((data[i] & 0x03) << 4) + ((data[i + 1] & 0xf0) >> 4)];

			if(i + 2u < size) {
				result += base64Table[((data[i + 1] & 0x0f) << 2) + ((data[i + 2] & 0xc0) >> 6)];
				result += base64Table[data[i + 2] & 0x3f];
			}
			else {
				result += base64Table[(data[i + 1] & 0x0f) << 2];
				result += '=';
			}
		}
		else {
			result += base64Table[(data[i] & 0x03) << 4];
			result += '=';
			result += '=';
		}
	}

	return result;
}

std::string decodeBase64(const std::string& data) {
	const auto decode = [](char c) -> unsigned int {
		if(c >= 'A' && c <= 'Z') {
			return c - 'A';
		}
		else if(c >= 'a' && c <= 'z') {
			return c - 'a' + ('Z' - 'A') + 1;
		}
		else if(c >= '0' && c <= '9') {
			return c - '0' + ('Z' - 'A') + ('z' - 'a') + 2u;
		}
		else if(c == '+') {
			return 62u;
		}
		else if(c == '/') {
			return 63u;
		}

		throw std::runtime_error("base64 decoding error");
	};

	std::string result;
	result.reserve(data.size() / 4u * 3u);

	for(std::size_t i = 0u; i < data.size(); i += 4u) {
		result += static_cast<char>((decode(data[i]) << 2) + ((decode(data[i + 1]) & 0x30) >> 4));

		if(i + 2u < data.size() && data[i + 2] != '=') {
			result += static_cast<char>(((decode(data[i + 1]) & 0x0f) << 4) + ((decode(data[i + 2]) & 0x3c) >> 2));

			if(i + 3u < data.size() && data[i + 3] != '=') {
				result += static_cast<char>(((decode(data[i + 2]) & 0x03) << 6) + decode(data[i + 3]));
			}
		}
	}

	return result;
}
}