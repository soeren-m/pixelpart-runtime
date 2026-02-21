#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <cmath>

namespace pixelpart {
namespace math {
template <typename T>
T pow(T base, T exp) {
	return std::pow(base, exp);
}
template <typename T>
vector2<T> pow(const vector2<T>& base, const vector2<T>& exp) {
	return vector2<T>(
		std::pow(base.x, exp.x),
		std::pow(base.y, exp.y));
}
template <typename T>
vector3<T> pow(const vector3<T>& base, const vector3<T>& exp) {
	return vector3<T>(
		std::pow(base.x, exp.x),
		std::pow(base.y, exp.y),
		std::pow(base.z, exp.z));
}
template <typename T>
vector4<T> pow(const vector4<T>& base, const vector4<T>& exp) {
	return vector4<T>(
		std::pow(base.x, exp.x),
		std::pow(base.y, exp.y),
		std::pow(base.z, exp.z),
		std::pow(base.w, exp.w));
}

template <typename T>
T exp(T v) {
	return std::exp(v);
}
template <typename T>
vector2<T> exp(const vector2<T>& v) {
	return vector2<T>(
		std::exp(v.x),
		std::exp(v.y));
}
template <typename T>
vector3<T> exp(const vector3<T>& v) {
	return vector3<T>(
		std::exp(v.x),
		std::exp(v.y),
		std::exp(v.z));
}
template <typename T>
vector4<T> exp(const vector4<T>& v) {
	return vector4<T>(
		std::exp(v.x),
		std::exp(v.y),
		std::exp(v.z),
		std::exp(v.w));
}

template <typename T>
T exp2(T v) {
	return std::exp2(v);
}
template <typename T>
vector2<T> exp2(const vector2<T>& v) {
	return vector2<T>(
		std::exp2(v.x),
		std::exp2(v.y));
}
template <typename T>
vector3<T> exp2(const vector3<T>& v) {
	return vector3<T>(
		std::exp2(v.x),
		std::exp2(v.y),
		std::exp2(v.z));
}
template <typename T>
vector4<T> exp2(const vector4<T>& v) {
	return vector4<T>(
		std::exp2(v.x),
		std::exp2(v.y),
		std::exp2(v.z),
		std::exp2(v.w));
}

template <typename T>
T log(T v) {
	return std::log(v);
}
template <typename T>
vector2<T> log(const vector2<T>& v) {
	return vector2<T>(
		std::log(v.x),
		std::log(v.y));
}
template <typename T>
vector3<T> log(const vector3<T>& v) {
	return vector3<T>(
		std::log(v.x),
		std::log(v.y),
		std::log(v.z));
}
template <typename T>
vector4<T> log(const vector4<T>& v) {
	return vector4<T>(
		std::log(v.x),
		std::log(v.y),
		std::log(v.z),
		std::log(v.w));
}

template <typename T>
T log2(T v) {
	return std::log2(v);
}
template <typename T>
vector2<T> log2(const vector2<T>& v) {
	return vector2<T>(
		std::log2(v.x),
		std::log2(v.y));
}
template <typename T>
vector3<T> log2(const vector3<T>& v) {
	return vector3<T>(
		std::log2(v.x),
		std::log2(v.y),
		std::log2(v.z));
}
template <typename T>
vector4<T> log2(const vector4<T>& v) {
	return vector4<T>(
		std::log2(v.x),
		std::log2(v.y),
		std::log2(v.z),
		std::log2(v.w));
}

template <typename T>
T sqrt(T v) {
	return std::sqrt(v);
}
template <typename T>
vector2<T> sqrt(const vector2<T>& v) {
	return vector2<T>(
		std::sqrt(v.x),
		std::sqrt(v.y));
}
template <typename T>
vector3<T> sqrt(const vector3<T>& v) {
	return vector3<T>(
		std::sqrt(v.x),
		std::sqrt(v.y),
		std::sqrt(v.z));
}
template <typename T>
vector4<T> sqrt(const vector4<T>& v) {
	return vector4<T>(
		std::sqrt(v.x),
		std::sqrt(v.y),
		std::sqrt(v.z),
		std::sqrt(v.w));
}
}
}
