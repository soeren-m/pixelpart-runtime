#include "DeserializationException.h"

namespace pixelpart {
DeserializationException::DeserializationException(const char* msg) : std::runtime_error(msg) {

}
DeserializationException::DeserializationException(const std::string& msg) : std::runtime_error(msg) {

}
}