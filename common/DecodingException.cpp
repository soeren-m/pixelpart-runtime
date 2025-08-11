#include "DecodingException.h"

namespace pixelpart {
DecodingException::DecodingException(const char* msg) : std::runtime_error(msg) {

}
DecodingException::DecodingException(const std::string& msg) : std::runtime_error(msg) {

}
}
