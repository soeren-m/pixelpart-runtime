#include "StringUtil.h"
#include <cstddef>

namespace pixelpart {
std::string replaceString(std::string str, const std::string& to, const std::string& from) {
	if(from.empty()) {
		return str;
	}

	std::size_t pos = 0;
	while((pos = str.find(from, pos)) != std::string::npos) {
		str.replace(pos, from.length(), to);
		pos += to.length();
	}

	return str;
}
}
