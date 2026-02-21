#include "Color.h"
#include <cstdint>

namespace pixelpart {
float4_t rgb2hsv(const float4_t& rgba) {
	float4_t result(0.0, 0.0, 0.0, rgba.w);

	float_t min = rgba.x < rgba.y ? rgba.x : rgba.y;
	min = min < rgba.z ? min : rgba.z;

	float_t max = rgba.x > rgba.y ? rgba.x : rgba.y;
	max = max > rgba.z ? max : rgba.z;
	result.z = max;

	float_t delta = max - min;
	if(delta < 0.00001) {
		result.y = 0.0;
		result.x = 0.0;

		return result;
	}

	if(max > 0.0) {
		result.y = delta / max;
	}
	else {
		result.y = 0.0;
		result.x = 0.0;

		return result;
	}

	if(rgba.x >= max) {
		result.x = (rgba.y - rgba.z) / delta;
	}
	else {
		if(rgba.y >= max) {
			result.x = 2.0 + (rgba.z - rgba.x) / delta;
		}
		else {
			result.x = 4.0 + (rgba.x - rgba.y) / delta;
		}
	}

	result.x *= 60.0;

	if(result.x < 0.0) {
		result.x += 360.0;
	}

	return result;
}

float4_t hsv2rgb(const float4_t& hsva) {
	if(hsva.y <= 0.0) {
		return float4_t(hsva.z, hsva.z, hsva.z, hsva.w);
	}

	float_t hh = hsva.x;
	if(hh >= 360.0) {
		hh = 0.0;
	}

	hh /= 60.0;

	std::uint32_t phase = static_cast<std::uint32_t>(hh);
	float_t ff = hh - phase;
	float_t p = hsva.z * (1.0 - hsva.y);
	float_t q = hsva.z * (1.0 - (hsva.y * ff));
	float_t t = hsva.z * (1.0 - (hsva.y * (1.0 - ff)));

	switch(phase) {
		case 0:
			return float4_t(hsva.z, t, p, hsva.w);
		case 1:
			return float4_t(q, hsva.z, p, hsva.w);
		case 2:
			return float4_t(p, hsva.z, t, hsva.w);
		case 3:
			return float4_t(p, q, hsva.z, hsva.w);
		case 4:
			return float4_t(t, p, hsva.z, hsva.w);
		default:
			return float4_t(hsva.z, p, q, hsva.w);
	}

	return float4_t(hsva.z, p, q, hsva.w);
}
}
