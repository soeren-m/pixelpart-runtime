#include "CompressionException.h"

namespace pixelpart {
CompressionException::CompressionException(const char* msg) : std::runtime_error(msg) {

}
CompressionException::CompressionException(const std::string& msg) : std::runtime_error(msg) {

}
}
