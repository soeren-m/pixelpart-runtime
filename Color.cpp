#include "Color.h"

namespace pixelpart {
vec4d rgb2hsv(const vec4d& colorRGB) {
	vec4d result(0.0, 0.0, 0.0, colorRGB.a);

	floatd min = colorRGB.r < colorRGB.g ? colorRGB.r : colorRGB.g;
	min = min < colorRGB.b ? min : colorRGB.b;

	floatd max = colorRGB.r > colorRGB.g ? colorRGB.r : colorRGB.g;
	max = max > colorRGB.b ? max : colorRGB.b;
	result.z = max;

	floatd delta = max - min;
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

vec4d hsv2rgb(const vec4d& colorHSV) {
	if(colorHSV.y <= 0.0) {
		return vec4d(colorHSV.z, colorHSV.z, colorHSV.z, colorHSV.a);
	}

	floatd hh = colorHSV.x;
	if(hh >= 360.0) {
		hh = 0.0;
	}

	hh /= 60.0;

	uint32_t i = static_cast<uint32_t>(hh);
	floatd ff = hh - i;
	floatd p = colorHSV.z * (1.0 - colorHSV.y);
	floatd q = colorHSV.z * (1.0 - (colorHSV.y * ff));
	floatd t = colorHSV.z * (1.0 - (colorHSV.y * (1.0 - ff)));

	switch(i) {
		case 0:
			return vec4d(colorHSV.z, t, p, colorHSV.a);
		case 1:
			return vec4d(q, colorHSV.z, p, colorHSV.a);
		case 2:
			return vec4d(p, colorHSV.z, t, colorHSV.a);
		case 3:
			return vec4d(p, q, colorHSV.z, colorHSV.a);
		case 4:
			return vec4d(t, p, colorHSV.z, colorHSV.a);
		default:
			return vec4d(colorHSV.z, p, q, colorHSV.a);
	}

	return vec4d(colorHSV.z, p, q, colorHSV.a);
}

vec4d hsvAdd(const vec4d& colorRGB, const vec4d& colorHSV, floatd alpha) {
	vec4d hsv = rgb2hsv(colorRGB);

	floatd hue = std::fmod(hsv.x + colorHSV.x, 360.0);
	hsv.x = (hue < 0.0) ? hue + 360.0 : hue;
	hsv.y = glm::clamp(hsv.y + colorHSV.y, 0.0, 1.0);
	hsv.z = glm::clamp(hsv.z + colorHSV.z, 0.0, 1.0);
	hsv.w = glm::clamp(hsv.w * colorHSV.w * alpha, 0.0, 1.0);

	return hsv2rgb(hsv);
}
}