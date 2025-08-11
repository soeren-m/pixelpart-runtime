#include "Types.h"

namespace pixelpart {
pixelpart::int_t* valuePtr(pixelpart::int_t& v) {
	return &v;
}
pixelpart::float_t* valuePtr(pixelpart::float_t& v) {
	return &v;
}

const pixelpart::int_t* valuePtr(const pixelpart::int_t& v) {
	return &v;
}
const pixelpart::float_t* valuePtr(const pixelpart::float_t& v) {
	return &v;
}
}
