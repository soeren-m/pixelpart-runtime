#pragma once

#include <string>
#include <unordered_map>

namespace pixelpart {
template <typename T>
using ResourceMap = std::unordered_map<std::string, T>;
}
