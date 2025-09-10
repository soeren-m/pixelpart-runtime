#include "Types.h"

namespace pixelpart {
int_t* valuePtr(int_t& v) {
	return &v;
}
float_t* valuePtr(float_t& v) {
	return &v;
}

const int_t* valuePtr(const int_t& v) {
	return &v;
}
const float_t* valuePtr(const float_t& v) {
	return &v;
}
}
