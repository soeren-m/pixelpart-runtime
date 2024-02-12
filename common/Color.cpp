#include "Color.h"

namespace pixelpart {
vec4_t rgb2hsv(const vec4_t& colorRGB) {
	vec4_t result(0.0, 0.0, 0.0, colorRGB.a);

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

vec4_t hsv2rgb(const vec4_t& colorHSV) {
	if(colorHSV.y <= 0.0) {
		return vec4_t(colorHSV.z, colorHSV.z, colorHSV.z, colorHSV.a);
	}

	float_t hh = colorHSV.x;
	if(hh >= 360.0) {
		hh = 0.0;
	}

	hh /= 60.0;

	uint32_t i = static_cast<uint32_t>(hh);
	float_t ff = hh - i;
	float_t p = colorHSV.z * (1.0 - colorHSV.y);
	float_t q = colorHSV.z * (1.0 - (colorHSV.y * ff));
	float_t t = colorHSV.z * (1.0 - (colorHSV.y * (1.0 - ff)));

	switch(i) {
		case 0:
			return vec4_t(colorHSV.z, t, p, colorHSV.a);
		case 1:
			return vec4_t(q, colorHSV.z, p, colorHSV.a);
		case 2:
			return vec4_t(p, colorHSV.z, t, colorHSV.a);
		case 3:
			return vec4_t(p, q, colorHSV.z, colorHSV.a);
		case 4:
			return vec4_t(t, p, colorHSV.z, colorHSV.a);
		default:
			return vec4_t(colorHSV.z, p, q, colorHSV.a);
	}

	return vec4_t(colorHSV.z, p, q, colorHSV.a);
}

vec4_t hsvAdd(const vec4_t& colorRGB, const vec4_t& colorHSV, float_t alpha) {
	vec4_t hsv = rgb2hsv(colorRGB);

	float_t hue = std::fmod(hsv.x + colorHSV.x, 360.0);
	hsv.x = (hue < 0.0) ? hue + 360.0 : hue;
	hsv.y = glm::clamp(hsv.y + colorHSV.y, 0.0, 1.0);
	hsv.z = glm::clamp(hsv.z + colorHSV.z, 0.0, 1.0);
	hsv.w = hsv.w * colorHSV.w * alpha;

	return hsv2rgb(hsv);
}
}