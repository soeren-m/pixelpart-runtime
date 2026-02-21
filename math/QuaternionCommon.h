#pragma once

#include "Quaternion.h"
#include <cmath>

namespace pixelpart {
namespace math {
template <typename T>
T lengthSquared(const quaternion<T>& q) {
	return q.x * q.x +
		q.y * q.y +
		q.z * q.z +
		q.w * q.w;
}

template <typename T>
T length(const quaternion<T>& q) {
	return std::sqrt(lengthSquared(q));
}

template <typename T>
quaternion<T> normalize(const quaternion<T>& q) {
	T l = length(q);
	if(l <= T(0)) {
		return quaternion<T>(T(0), T(0), T(0), T(1));
	}

	return q / l;
}

template <typename T>
T dot(const quaternion<T>& q1, const quaternion<T>& q2) {
	return q1.x * q2.x +
		q1.y * q2.y +
		q1.z * q2.z +
		q1.w * q2.w;
}

template <typename T>
quaternion<T> cross(const quaternion<T>& q1, const quaternion<T>& q2) {
	return quaternion<T>(
		q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
		q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
		q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x,
		q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z);
}

template <typename T>
quaternion<T> conjugate(const quaternion<T>& q) {
	return quaternion<T>(-q.x, -q.y, -q.z, q.w);
}

template <typename T>
quaternion<T> inverse(const quaternion<T>& q) {
	return conjugate(q) / lengthSquared(q);
}
}
}
