#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Trigonometry.h"
#include <cmath>
#include <algorithm>

namespace pixelpart {
namespace math {
template <typename T>
T lengthSquared(T v) {
	return v * v;
}
template <typename T>
T lengthSquared(const vector2<T>& v) {
	return v.x * v.x +
		v.y * v.y;
}
template <typename T>
T lengthSquared(const vector3<T>& v) {
	return v.x * v.x +
		v.y * v.y +
		v.z * v.z;
}
template <typename T>
T lengthSquared(const vector4<T>& v) {
	return v.x * v.x +
		v.y * v.y +
		v.z * v.z +
		v.w * v.w;
}

template <typename T>
T length(T v) {
	return std::abs(v);
}
template <typename T>
T length(const vector2<T>& v) {
	return std::sqrt(lengthSquared(v));
}
template <typename T>
T length(const vector3<T>& v) {
	return std::sqrt(lengthSquared(v));
}
template <typename T>
T length(const vector4<T>& v) {
	return std::sqrt(lengthSquared(v));
}

template <typename T>
T distance(T v1, T v2) {
	return length(v2 - v1);
}
template <typename T>
T distance(const vector2<T>& v1, const vector2<T>& v2) {
	return length(v2 - v1);
}
template <typename T>
T distance(const vector3<T>& v1, const vector3<T>& v2) {
	return length(v2 - v1);
}
template <typename T>
T distance(const vector4<T>& v1, const vector4<T>& v2) {
	return length(v2 - v1);
}

template <typename T>
T distanceSquared(T v1, T v2) {
	return lengthSquared(v2 - v1);
}
template <typename T>
T distanceSquared(const vector2<T>& v1, const vector2<T>& v2) {
	return lengthSquared(v2 - v1);
}
template <typename T>
T distanceSquared(const vector3<T>& v1, const vector3<T>& v2) {
	return lengthSquared(v2 - v1);
}
template <typename T>
T distanceSquared(const vector4<T>& v1, const vector4<T>& v2) {
	return lengthSquared(v2 - v1);
}

template <typename TVec>
TVec normalize(const TVec& v) {
	return v / length(v);
}

template <typename T>
T dot(T v1, T v2) {
	return v1 * v2;
}
template <typename T>
T dot(const vector2<T>& v1, const vector2<T>& v2) {
	return v1.x * v2.x +
		v1.y * v2.y;
}
template <typename T>
T dot(const vector3<T>& v1, const vector3<T>& v2) {
	return v1.x * v2.x +
		v1.y * v2.y +
		v1.z * v2.z;
}
template <typename T>
T dot(const vector4<T>& v1, const vector4<T>& v2) {
	return v1.x * v2.x +
		v1.y * v2.y +
		v1.z * v2.z +
		v1.w * v2.w;
}

template <typename T>
vector3<T> cross(const vector3<T>& v1, const vector3<T>& v2) {
	return vector3<T>(
		v1.y * v2.z - v2.y * v1.z,
		v1.z * v2.x - v2.z * v1.x,
		v1.x * v2.y - v2.x * v1.y);
}

template <typename TVec>
TVec reflect(const TVec& incident, const TVec& normal) {
	return incident - dot(normal, incident) * normal * static_cast<typename TVec::ValueType>(2);
}

template <typename T>
vector2<T> rotateVector(const vector2<T>& v, T angle) {
	T c = std::cos(angle);
	T s = std::sin(angle);

	vector2<T> result;
	result.x = v.x * c - v.y * s;
	result.y = v.x * s + v.y * c;

	return result;
}
template <typename T>
vector2<T> rotatePoint(const vector2<T>& p, const vector2<T>& pivot, T angle) {
	T c = std::cos(angle);
	T s = std::sin(angle);

	vector2<T> result;
	result.x = pivot.x + (p.x - pivot.x) * c - (p.y - pivot.y) * s;
	result.y = pivot.y + (p.x - pivot.x) * s + (p.y - pivot.y) * c;

	return result;
}

template <typename TVec>
typename TVec::ValueType absoluteAngle(const TVec& v1, const TVec& v2) {
	return std::acos(std::clamp(
		dot(v1, v2),
		typename TVec::ValueType(-1),
		typename TVec::ValueType(1)));
}

template <typename T>
T orientedAngle(const vector2<T>& v1, const vector2<T>& v2) {
	T angle = absoluteAngle(v1, v2);
	T partialCross = v1.x * v2.y - v2.x * v1.y;

	return partialCross > T(0)
		? angle
		: -angle;
}
template <typename T>
T orientedAngle(const vector3<T>& v1, const vector3<T>& v2, const vector3<T>& ref) {
	T angle = absoluteAngle(v1, v2);

	return dot(ref, cross(v1, v2)) > T(0)
		? angle
		: -angle;
}
}
}
