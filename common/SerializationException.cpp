#include "SerializationException.h"

namespace pixelpart {
SerializationException::SerializationException(const char* msg) : std::runtime_error(msg) {

}
SerializationException::SerializationException(const std::string& msg) : std::runtime_error(msg) {

}
}
