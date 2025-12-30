#pragma once

#include <cstddef>
#include <string>
#include <charconv>

#ifdef __clang__
#include <sstream>
#include <iomanip>
#endif

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
#ifdef __clang__
	std::stringstream stream;
	stream.imbue(std::locale::classic());
	stream << std::setprecision(precision) << value;

	return stream.str();

#else
	static const std::size_t bufferSize = 128;
	static char buffer[bufferSize];

	std::to_chars_result result =
		std::to_chars(buffer, buffer + bufferSize, value, std::chars_format::general, precision);

	return result.ec == std::errc()
		? std::string(buffer, result.ptr - buffer)
		: std::string();

#endif
}

template <typename T>
T deserializeFloat(const std::string& str, T defaultValue = T(0)) {
	T value = defaultValue;

#ifdef __clang__
	std::stringstream stream;
	stream.imbue(std::locale::classic());
	stream << str;
	stream >> value;

#else
	std::from_chars_result result =
		std::from_chars(str.data(), str.data() + str.size(), value, std::chars_format::general);

#endif

	return value;
}
}
