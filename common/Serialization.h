#pragma once

#include <cstddef>
#include <string>
#include <charconv>

namespace pixelpart {
template <typename T>
std::string serializeInt(T value) {
	static const std::size_t bufferSize = 128;
	static char buffer[bufferSize];

	std::to_chars_result result =
		std::to_chars(buffer, buffer + bufferSize, value);

	return result.ec == std::errc()
		? std::string(buffer, result.ptr - buffer)
		: std::string();
}

template <typename T>
T deserializeInt(const std::string& str, T defaultValue = T(0)) {
	T value = defaultValue;
	std::from_chars_result result =
		std::from_chars(str.data(), str.data() + str.size(), value);

	return value;
}

template <typename T>
std::string serializeFloat(T value, int precision) {
	static const std::size_t bufferSize = 128;
	static char buffer[bufferSize];

	std::to_chars_result result =
		std::to_chars(buffer, buffer + bufferSize, value, std::chars_format::general, precision);

	return result.ec == std::errc()
		? std::string(buffer, result.ptr - buffer)
		: std::string();
}

template <typename T>
T deserializeFloat(const std::string& str, T defaultValue = T(0)) {
	T value = defaultValue;
	std::from_chars_result result =
		std::from_chars(str.data(), str.data() + str.size(), value, std::chars_format::general);

	return value;
}
}
