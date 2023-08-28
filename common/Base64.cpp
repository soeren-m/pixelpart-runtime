#include "Base64.h"
#include <stdexcept>

namespace pixelpart {
static const char* base64Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string encodeBase64(const unsigned char* data, std::size_t size) {
	std::string result;
	result.reserve((size + 2) / 3 * 4);

	for(std::size_t i = 0; i < size; i += 3) {
		result += base64Table[(data[i + 0] & 0xfc) >> 2];

		if(i + 1 < size) {
			result += base64Table[((data[i + 0] & 0x03) << 4) + ((data[i + 1] & 0xf0) >> 4)];

			if(i + 2 < size) {
				result += base64Table[((data[i + 1] & 0x0f) << 2) + ((data[i + 2] & 0xc0) >> 6)];
				result += base64Table[data[i + 2] & 0x3f];
			}
			else {
				result += base64Table[(data[i + 1] & 0x0f) << 2];
				result += '=';
			}
		}
		else {
			result += base64Table[(data[i + 0] & 0x03) << 4];
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
			return c - '0' + ('Z' - 'A') + ('z' - 'a') + 2;
		}
		else if(c == '+') {
			return 62;
		}
		else if(c == '/') {
			return 63;
		}

		throw std::runtime_error("base64 decoding error");
	};

	std::string result;
	result.reserve(data.size() / 4 * 3);

	for(std::size_t i = 0; i < data.size(); i += 4) {
		result += static_cast<char>((decode(data[i + 0]) << 2) + ((decode(data[i + 1]) & 0x30) >> 4));

		if(i + 2 < data.size() && data[i + 2] != '=') {
			result += static_cast<char>(((decode(data[i + 1]) & 0x0f) << 4) + ((decode(data[i + 2]) & 0x3c) >> 2));

			if(i + 3 < data.size() && data[i + 3] != '=') {
				result += static_cast<char>(((decode(data[i + 2]) & 0x03) << 6) + decode(data[i + 3]));
			}
		}
	}

	return result;
}
}