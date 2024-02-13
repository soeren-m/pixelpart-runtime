#include "StringUtil.h"

namespace pixelpart {
std::string replace(std::string str, const std::string& to, const std::string& from) {
	if(from.empty()) {
		return str;
	}

	std::size_t pos = 0u;
	while((pos = str.find(from, pos)) != std::string::npos) {
		str.replace(pos, from.length(), to);
		pos += to.length();
	}

	return str;
}
}