#pragma once

#include <string>
#include <memory>
#include <stdexcept>

namespace pixelpart {
template <typename... Args>
std::string formatString(const char* str, Args... args) {
	int size = snprintf(nullptr, 0, str, args...) + 1;
	if(size <= 0) {
		throw std::runtime_error("Formatting error");
	}

	std::unique_ptr<char[]> buffer(new char[size]);
	snprintf(buffer.get(), size, str, args...);

	return std::string(buffer.get(), buffer.get() + size - 1);
}

template <typename... Args>
std::string formatString(const std::string& str, Args... args) {
	return formatString(str.c_str(), std::forward<Args>(args)...);
}

std::string replaceString(std::string str, const std::string& to, const std::string& from);
}
