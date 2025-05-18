#include "Base64.h"
#include "DecodingException.h"

namespace pixelpart {
std::string encodeBase64(const std::uint8_t* data, std::size_t size) {
	static const char* base64Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	std::string result;
	result.reserve((size + 2) / 3 * 4);

	for(std::size_t byteIndex = 0; byteIndex < size; byteIndex += 3) {
		result += base64Table[(data[byteIndex] & 0xfc) >> 2];

		if(byteIndex + 1 < size) {
			result += base64Table[((data[byteIndex] & 0x03) << 4) + ((data[byteIndex + 1] & 0xf0) >> 4)];

			if(byteIndex + 2 < size) {
				result += base64Table[((data[byteIndex + 1] & 0x0f) << 2) + ((data[byteIndex + 2] & 0xc0) >> 6)];
				result += base64Table[data[byteIndex + 2] & 0x3f];
			}
			else {
				result += base64Table[(data[byteIndex + 1] & 0x0f) << 2];
				result += '=';
			}
		}
		else {
			result += base64Table[(data[byteIndex] & 0x03) << 4];
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

		throw DecodingException("Unknown base64 character");
	};

	std::string result;
	result.reserve(data.size() / 4 * 3);

	for(std::size_t byteIndex = 0; byteIndex < data.size(); byteIndex += 4) {
		result += static_cast<char>((decode(data[byteIndex]) << 2) + ((decode(data[byteIndex + 1]) & 0x30) >> 4));

		if(byteIndex + 2 < data.size() && data[byteIndex + 2] != '=') {
			result += static_cast<char>(((decode(data[byteIndex + 1]) & 0x0f) << 4) + ((decode(data[byteIndex + 2]) & 0x3c) >> 2));

			if(byteIndex + 3 < data.size() && data[byteIndex + 3] != '=') {
				result += static_cast<char>(((decode(data[byteIndex + 2]) & 0x03) << 6) + decode(data[byteIndex + 3]));
			}
		}
	}

	return result;
}
}