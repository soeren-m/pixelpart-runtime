#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <cmath>
#include <algorithm>

namespace pixelpart {
namespace math {
template <typename T>
T abs(T v) {
	return std::abs(v);
}
template <typename T>
vector2<T> abs(const vector2<T>& v) {
	return vector2<T>(
		std::abs(v.x),
		std::abs(v.y));
}
template <typename T>
vector3<T> abs(const vector3<T>& v) {
	return vector3<T>(
		std::abs(v.x),
		std::abs(v.y),
		std::abs(v.z));
}
template <typename T>
vector4<T> abs(const vector4<T>& v) {
	return vector4<T>(
		std::abs(v.x),
		std::abs(v.y),
		std::abs(v.z),
		std::abs(v.w));
}

template <typename T>
T sign(T v) {
	return T((T(0) < v) - (v < T(0)));
}
template <typename T>
vector2<T> sign(const vector2<T>& v) {
	return vector2<T>(
		sign(v.x),
		sign(v.y));
}
template <typename T>
vector3<T> sign(const vector3<T>& v) {
	return vector3<T>(
		sign(v.x),
		sign(v.y),
		sign(v.z));
}
template <typename T>
vector4<T> sign(const vector4<T>& v) {
	return vector4<T>(
		sign(v.x),
		sign(v.y),
		sign(v.z),
		sign(v.w));
}

template <typename T>
T floor(T v) {
	return std::floor(v);
}
template <typename T>
vector2<T> floor(const vector2<T>& v) {
	return vector2<T>(
		std::floor(v.x),
		std::floor(v.y));
}
template <typename T>
vector3<T> floor(const vector3<T>& v) {
	return vector3<T>(
		std::floor(v.x),
		std::floor(v.y),
		std::floor(v.z));
}
template <typename T>
vector4<T> floor(const vector4<T>& v) {
	return vector4<T>(
		std::floor(v.x),
		std::floor(v.y),
		std::floor(v.z),
		std::floor(v.w));
}

template <typename T>
T ceil(T v) {
	return std::ceil(v);
}
template <typename T>
vector2<T> ceil(const vector2<T>& v) {
	return vector2<T>(
		std::ceil(v.x),
		std::ceil(v.y));
}
template <typename T>
vector3<T> ceil(const vector3<T>& v) {
	return vector3<T>(
		std::ceil(v.x),
		std::ceil(v.y),
		std::ceil(v.z));
}
template <typename T>
vector4<T> ceil(const vector4<T>& v) {
	return vector4<T>(
		std::ceil(v.x),
		std::ceil(v.y),
		std::ceil(v.z),
		std::ceil(v.w));
}

template <typename T>
T round(T v) {
	return std::round(v);
}
template <typename T>
vector2<T> round(const vector2<T>& v) {
	return vector2<T>(
		std::round(v.x),
		std::round(v.y));
}
template <typename T>
vector3<T> round(const vector3<T>& v) {
	return vector3<T>(
		std::round(v.x),
		std::round(v.y),
		std::round(v.z));
}
template <typename T>
vector4<T> round(const vector4<T>& v) {
	return vector4<T>(
		std::round(v.x),
		std::round(v.y),
		std::round(v.z),
		std::round(v.w));
}

template <typename TVec>
TVec fract(const TVec& v) {
	return v - floor(v);
}

template <typename T>
T min(T v1, T v2) {
	return std::min(v1, v2);
}
template <typename T>
vector2<T> min(const vector2<T>& v1, const vector2<T>& v2) {
	return vector2<T>(
		std::min(v1.x, v2.x),
		std::min(v1.y, v2.y));
}
template <typename T>
vector3<T> min(const vector3<T>& v1, const vector3<T>& v2) {
	return vector3<T>(
		std::min(v1.x, v2.x),
		std::min(v1.y, v2.y),
		std::min(v1.z, v2.z));
}
template <typename T>
vector4<T> min(const vector4<T>& v1, const vector4<T>& v2) {
	return vector4<T>(
		std::min(v1.x, v2.x),
		std::min(v1.y, v2.y),
		std::min(v1.z, v2.z),
		std::min(v1.w, v2.w));
}

template <typename T>
T max(T v1, T v2) {
	return std::max(v1, v2);
}
template <typename T>
vector2<T> max(const vector2<T>& v1, const vector2<T>& v2) {
	return vector2<T>(
		std::max(v1.x, v2.x),
		std::max(v1.y, v2.y));
}
template <typename T>
vector3<T> max(const vector3<T>& v1, const vector3<T>& v2) {
	return vector3<T>(
		std::max(v1.x, v2.x),
		std::max(v1.y, v2.y),
		std::max(v1.z, v2.z));
}
template <typename T>
vector4<T> max(const vector4<T>& v1, const vector4<T>& v2) {
	return vector4<T>(
		std::max(v1.x, v2.x),
		std::max(v1.y, v2.y),
		std::max(v1.z, v2.z),
		std::max(v1.w, v2.w));
}

template <typename T>
T clamp(T v, T min, T max) {
	return std::clamp(v, min, max);
}
template <typename T>
vector2<T> clamp(const vector2<T>& v, const vector2<T>& min, const vector2<T>& max) {
	return vector2<T>(
		std::clamp(v.x, min.x, max.x),
		std::clamp(v.y, min.y, max.y));
}
template <typename T>
vector3<T> clamp(const vector3<T>& v, const vector3<T>& min, const vector3<T>& max) {
	return vector3<T>(
		std::clamp(v.x, min.x, max.x),
		std::clamp(v.y, min.y, max.y),
		std::clamp(v.z, min.z, max.z));
}
template <typename T>
vector4<T> clamp(const vector4<T>& v, const vector4<T>& min, const vector4<T>& max) {
	return vector4<T>(
		std::clamp(v.x, min.x, max.x),
		std::clamp(v.y, min.y, max.y),
		std::clamp(v.z, min.z, max.z),
		std::clamp(v.w, min.w, max.w));
}

template <typename T>
T mod(T x, T y) {
	return x - y * std::floor(x / y);
}
template <typename T>
vector2<T> mod(const vector2<T>& v1, const vector2<T>& v2) {
	return vector2<T>(
		mod(v1.x, v2.x),
		mod(v1.y, v2.y));
}
template <typename T>
vector3<T> mod(const vector3<T>& v1, const vector3<T>& v2) {
	return vector3<T>(
		mod(v1.x, v2.x),
		mod(v1.y, v2.y),
		mod(v1.z, v2.z));
}
template <typename T>
vector4<T> mod(const vector4<T>& v1, const vector4<T>& v2) {
	return vector4<T>(
		mod(v1.x, v2.x),
		mod(v1.y, v2.y),
		mod(v1.z, v2.z),
		mod(v1.w, v2.w));
}
}
}
