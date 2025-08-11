#include "Color.h"
#include <cstdint>

namespace pixelpart {
float4_t rgb2hsv(const float4_t& colorRGB) {
	float4_t result(0.0, 0.0, 0.0, colorRGB.a);

	float_t min = colorRGB.r < colorRGB.g ? colorRGB.r : colorRGB.g;
	min = min < colorRGB.b ? min : colorRGB.b;

	float_t max = colorRGB.r > colorRGB.g ? colorRGB.r : colorRGB.g;
	max = max > colorRGB.b ? max : colorRGB.b;
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

	if(colorRGB.r >= max) {
		result.x = (colorRGB.g - colorRGB.b) / delta;
	}
	else {
		if(colorRGB.g >= max) {
			result.x = 2.0 + (colorRGB.b - colorRGB.r) / delta;
		}
		else {
			result.x = 4.0 + (colorRGB.r - colorRGB.g) / delta;
		}
	}

	result.x *= 60.0;

	if(result.x < 0.0) {
		result.x += 360.0;
	}

	return result;
}

float4_t hsv2rgb(const float4_t& colorHSV) {
	if(colorHSV.y <= 0.0) {
		return float4_t(colorHSV.z, colorHSV.z, colorHSV.z, colorHSV.a);
	}

	float_t hh = colorHSV.x;
	if(hh >= 360.0) {
		hh = 0.0;
	}

	hh /= 60.0;

	std::uint32_t phase = static_cast<std::uint32_t>(hh);
	float_t ff = hh - phase;
	float_t p = colorHSV.z * (1.0 - colorHSV.y);
	float_t q = colorHSV.z * (1.0 - (colorHSV.y * ff));
	float_t t = colorHSV.z * (1.0 - (colorHSV.y * (1.0 - ff)));

	switch(phase) {
		case 0:
			return float4_t(colorHSV.z, t, p, colorHSV.a);
		case 1:
			return float4_t(q, colorHSV.z, p, colorHSV.a);
		case 2:
			return float4_t(p, colorHSV.z, t, colorHSV.a);
		case 3:
			return float4_t(p, q, colorHSV.z, colorHSV.a);
		case 4:
			return float4_t(t, p, colorHSV.z, colorHSV.a);
		default:
			return float4_t(colorHSV.z, p, q, colorHSV.a);
	}

	return float4_t(colorHSV.z, p, q, colorHSV.a);
}
}
