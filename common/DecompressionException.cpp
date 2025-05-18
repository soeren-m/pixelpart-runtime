#include "DecompressionException.h"

namespace pixelpart {
DecompressionException::DecompressionException(const char* msg) : std::runtime_error(msg) {

}
DecompressionException::DecompressionException(const std::string& msg) : std::runtime_error(msg) {

}
}