#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <cmath>

namespace pixelpart {
namespace math {
template <typename T>
T radians(T deg) {
	return deg * static_cast<T>(0.01745329251994329576923690768489);
}
template <typename T>
vector2<T> radians(const vector2<T>& v) {
	return vector2<T>(
		radians(v.x),
		radians(v.y));
}
template <typename T>
vector3<T> radians(const vector3<T>& v) {
	return vector3<T>(
		radians(v.x),
		radians(v.y),
		radians(v.z));
}
template <typename T>
vector4<T> radians(const vector4<T>& v) {
	return vector4<T>(
		radians(v.x),
		radians(v.y),
		radians(v.z),
		radians(v.w));
}

template <typename T>
T degrees(T rad) {
	return rad * static_cast<T>(57.295779513082320876798154814105);
}
template <typename T>
vector2<T> degrees(const vector2<T>& v) {
	return vector2<T>(
		degrees(v.x),
		degrees(v.y));
}
template <typename T>
vector3<T> degrees(const vector3<T>& v) {
	return vector3<T>(
		degrees(v.x),
		degrees(v.y),
		degrees(v.z));
}
template <typename T>
vector4<T> degrees(const vector4<T>& v) {
	return vector4<T>(
		degrees(v.x),
		degrees(v.y),
		degrees(v.z),
		degrees(v.w));
}

template <typename T>
T sin(T v) {
	return std::sin(v);
}
template <typename T>
vector2<T> sin(const vector2<T>& v) {
	return vector2<T>(
		std::sin(v.x),
		std::sin(v.y));
}
template <typename T>
vector3<T> sin(const vector3<T>& v) {
	return vector3<T>(
		std::sin(v.x),
		std::sin(v.y),
		std::sin(v.z));
}
template <typename T>
vector4<T> sin(const vector4<T>& v) {
	return vector4<T>(
		std::sin(v.x),
		std::sin(v.y),
		std::sin(v.z),
		std::sin(v.w));
}

template <typename T>
T cos(T v) {
	return std::cos(v);
}
template <typename T>
vector2<T> cos(const vector2<T>& v) {
	return vector2<T>(
		std::cos(v.x),
		std::cos(v.y));
}
template <typename T>
vector3<T> cos(const vector3<T>& v) {
	return vector3<T>(
		std::cos(v.x),
		std::cos(v.y),
		std::cos(v.z));
}
template <typename T>
vector4<T> cos(const vector4<T>& v) {
	return vector4<T>(
		std::cos(v.x),
		std::cos(v.y),
		std::cos(v.z),
		std::cos(v.w));
}

template <typename T>
T tan(T v) {
	return std::tan(v);
}
template <typename T>
vector2<T> tan(const vector2<T>& v) {
	return vector2<T>(
		std::tan(v.x),
		std::tan(v.y));
}
template <typename T>
vector3<T> tan(const vector3<T>& v) {
	return vector3<T>(
		std::tan(v.x),
		std::tan(v.y),
		std::tan(v.z));
}
template <typename T>
vector4<T> tan(const vector4<T>& v) {
	return vector4<T>(
		std::tan(v.x),
		std::tan(v.y),
		std::tan(v.z),
		std::tan(v.w));
}

template <typename T>
T asin(T v) {
	return std::asin(v);
}
template <typename T>
vector2<T> asin(const vector2<T>& v) {
	return vector2<T>(
		std::asin(v.x),
		std::asin(v.y));
}
template <typename T>
vector3<T> asin(const vector3<T>& v) {
	return vector3<T>(
		std::asin(v.x),
		std::asin(v.y),
		std::asin(v.z));
}
template <typename T>
vector4<T> asin(const vector4<T>& v) {
	return vector4<T>(
		std::asin(v.x),
		std::asin(v.y),
		std::asin(v.z),
		std::asin(v.w));
}

template <typename T>
T acos(T v) {
	return std::acos(v);
}
template <typename T>
vector2<T> acos(const vector2<T>& v) {
	return vector2<T>(
		std::acos(v.x),
		std::acos(v.y));
}
template <typename T>
vector3<T> acos(const vector3<T>& v) {
	return vector3<T>(
		std::acos(v.x),
		std::acos(v.y),
		std::acos(v.z));
}
template <typename T>
vector4<T> acos(const vector4<T>& v) {
	return vector4<T>(
		std::acos(v.x),
		std::acos(v.y),
		std::acos(v.z),
		std::acos(v.w));
}

template <typename T>
T atan(T v) {
	return std::atan(v);
}
template <typename T>
vector2<T> atan(const vector2<T>& v) {
	return vector2<T>(
		std::atan(v.x),
		std::atan(v.y));
}
template <typename T>
vector3<T> atan(const vector3<T>& v) {
	return vector3<T>(
		std::atan(v.x),
		std::atan(v.y),
		std::atan(v.z));
}
template <typename T>
vector4<T> atan(const vector4<T>& v) {
	return vector4<T>(
		std::atan(v.x),
		std::atan(v.y),
		std::atan(v.z),
		std::atan(v.w));
}

template <typename T>
T atan2(T y, T x) {
	return std::atan2(y, x);
}
template <typename T>
vector2<T> atan2(const vector2<T>& y, const vector2<T>& x) {
	return vector2<T>(
		std::atan2(y.x, x.x),
		std::atan2(y.y, x.y));
}
template <typename T>
vector3<T> atan2(const vector2<T>& y, const vector2<T>& x) {
	return vector3<T>(
		std::atan2(y.x, x.x),
		std::atan2(y.y, x.y),
		std::atan2(y.z, x.z));
}
template <typename T>
vector4<T> atan2(const vector2<T>& y, const vector2<T>& x) {
	return vector4<T>(
		std::atan2(y.x, x.x),
		std::atan2(y.y, x.y),
		std::atan2(y.z, x.z),
		std::atan2(y.w, x.w));
}

template <typename T>
T sinh(T v) {
	return std::sinh(v);
}
template <typename T>
vector2<T> sinh(const vector2<T>& v) {
	return vector2<T>(
		std::sinh(v.x),
		std::sinh(v.y));
}
template <typename T>
vector3<T> sinh(const vector3<T>& v) {
	return vector3<T>(
		std::sinh(v.x),
		std::sinh(v.y),
		std::sinh(v.z));
}
template <typename T>
vector4<T> sinh(const vector4<T>& v) {
	return vector4<T>(
		std::sinh(v.x),
		std::sinh(v.y),
		std::sinh(v.z),
		std::sinh(v.w));
}

template <typename T>
T cosh(T v) {
	return std::cosh(v);
}
template <typename T>
vector2<T> cosh(const vector2<T>& v) {
	return vector2<T>(
		std::cosh(v.x),
		std::cosh(v.y));
}
template <typename T>
vector3<T> cosh(const vector3<T>& v) {
	return vector3<T>(
		std::cosh(v.x),
		std::cosh(v.y),
		std::cosh(v.z));
}
template <typename T>
vector4<T> cosh(const vector4<T>& v) {
	return vector4<T>(
		std::cosh(v.x),
		std::cosh(v.y),
		std::cosh(v.z),
		std::cosh(v.w));
}

template <typename T>
T tanh(T v) {
	return std::tanh(v);
}
template <typename T>
vector2<T> tanh(const vector2<T>& v) {
	return vector2<T>(
		std::tanh(v.x),
		std::tanh(v.y));
}
template <typename T>
vector3<T> tanh(const vector3<T>& v) {
	return vector3<T>(
		std::tanh(v.x),
		std::tanh(v.y),
		std::tanh(v.z));
}
template <typename T>
vector4<T> tanh(const vector4<T>& v) {
	return vector4<T>(
		std::tanh(v.x),
		std::tanh(v.y),
		std::tanh(v.z),
		std::tanh(v.w));
}

template <typename T>
T asinh(T v) {
	return std::asinh(v);
}
template <typename T>
vector2<T> asinh(const vector2<T>& v) {
	return vector2<T>(
		std::asinh(v.x),
		std::asinh(v.y));
}
template <typename T>
vector3<T> asinh(const vector3<T>& v) {
	return vector3<T>(
		std::asinh(v.x),
		std::asinh(v.y),
		std::asinh(v.z));
}
template <typename T>
vector4<T> asinh(const vector4<T>& v) {
	return vector4<T>(
		std::asinh(v.x),
		std::asinh(v.y),
		std::asinh(v.z),
		std::asinh(v.w));
}

template <typename T>
T acosh(T v) {
	return std::acosh(v);
}
template <typename T>
vector2<T> acosh(const vector2<T>& v) {
	return vector2<T>(
		std::acosh(v.x),
		std::acosh(v.y));
}
template <typename T>
vector3<T> acosh(const vector3<T>& v) {
	return vector3<T>(
		std::acosh(v.x),
		std::acosh(v.y),
		std::acosh(v.z));
}
template <typename T>
vector4<T> acosh(const vector4<T>& v) {
	return vector4<T>(
		std::acosh(v.x),
		std::acosh(v.y),
		std::acosh(v.z),
		std::acosh(v.w));
}

template <typename T>
T atanh(T v) {
	return std::atanh(v);
}
template <typename T>
vector2<T> atanh(const vector2<T>& v) {
	return vector2<T>(
		std::atanh(v.x),
		std::atanh(v.y));
}
template <typename T>
vector3<T> atanh(const vector3<T>& v) {
	return vector3<T>(
		std::atanh(v.x),
		std::atanh(v.y),
		std::atanh(v.z));
}
template <typename T>
vector4<T> atanh(const vector4<T>& v) {
	return vector4<T>(
		std::atanh(v.x),
		std::atanh(v.y),
		std::atanh(v.z),
		std::atanh(v.w));
}
}
}
