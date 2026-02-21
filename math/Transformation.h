#pragma once

#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Constants.h"
#include "Geometry.h"
#include <cmath>
#include <limits>

namespace pixelpart {
namespace math {
template <typename T>
matrix4x4<T> translationMatrix(const vector3<T>& v) {
	matrix4x4<T> result(T(1));
	result[3] = vector4<T>(v.x, v.y, v.z, T(1));

	return result;
}

template <typename T>
matrix4x4<T> scalingMatrix(const vector3<T>& v) {
	matrix4x4<T> result(T(1));
	result[0][0] = v.x;
	result[1][1] = v.y;
	result[2][2] = v.z;

	return result;
}

template <typename T>
matrix4x4<T> angleAxisRotationMatrix(T angle, const vector3<T>& axis) {
	T c = std::cos(angle);
	T s = std::sin(angle);

	vector3<T> a = normalize(axis);

	matrix4x4<T> result;
	result[0][0] = c + (T(1) - c) * a.x * a.x;
	result[0][1] = (T(1) - c) * a.x * a.y + s * a.z;
	result[0][2] = (T(1) - c) * a.x * a.z - s * a.y;
	result[0][3] = T(0);

	result[1][0] = (T(1) - c) * a.y * a.x - s * a.z;
	result[1][1] = c + (T(1) - c) * a.y * a.y;
	result[1][2] = (T(1) - c) * a.y * a.z + s * a.x;
	result[1][3] = T(0);

	result[2][0] = (T(1) - c) * a.z * a.x + s * a.y;
	result[2][1] = (T(1) - c) * a.z * a.y - s * a.x;
	result[2][2] = c + (T(1) - c) * a.z * a.z;
	result[2][3] = T(0);

	result[3] = vector4<T>(0, 0, 0, 1);

	return result;
}

template <typename T>
matrix4x4<T> quaternionRotationMatrix(const quaternion<T>& q) {
	T qxx(q.x * q.x);
	T qyy(q.y * q.y);
	T qzz(q.z * q.z);
	T qxz(q.x * q.z);
	T qxy(q.x * q.y);
	T qyz(q.y * q.z);
	T qwx(q.w * q.x);
	T qwy(q.w * q.y);
	T qwz(q.w * q.z);

	matrix4x4<T> result;
	result[0][0] = T(1) - T(2) * (qyy + qzz);
	result[0][1] = T(2) * (qxy + qwz);
	result[0][2] = T(2) * (qxz - qwy);
	result[0][3] = T(0);

	result[1][0] = T(2) * (qxy - qwz);
	result[1][1] = T(1) - T(2) * (qxx + qzz);
	result[1][2] = T(2) * (qyz + qwx);
	result[1][3] = T(0);

	result[2][0] = T(2) * (qxz + qwy);
	result[2][1] = T(2) * (qyz - qwx);
	result[2][2] = T(1) - T(2) * (qxx + qyy);
	result[2][3] = T(0);

	result[3] = vector4<T>(0, 0, 0, 1);

	return result;
}

template <typename T>
matrix4x4<T> yawPitchRollRotationMatrix(T yaw, T pitch, T roll) {
	T cosYaw = std::cos(yaw);
	T sinYaw = std::sin(yaw);
	T cosPitch = std::cos(pitch);
	T sinPitch = std::sin(pitch);
	T cosRoll = std::cos(roll);
	T sinRoll = std::sin(roll);

	matrix4x4<T> result;
	result[0][0] = cosYaw * cosRoll + sinYaw * sinPitch * sinRoll;
	result[0][1] = sinRoll * cosPitch;
	result[0][2] = -sinYaw * cosRoll + cosYaw * sinPitch * sinRoll;
	result[0][3] = T(0);

	result[1][0] = -cosYaw * sinRoll + sinYaw * sinPitch * cosRoll;
	result[1][1] = cosRoll * cosPitch;
	result[1][2] = sinRoll * sinYaw + cosYaw * sinPitch * cosRoll;
	result[1][3] = T(0);

	result[2][0] = sinYaw * cosPitch;
	result[2][1] = -sinPitch;
	result[2][2] = cosYaw * cosPitch;
	result[2][3] = T(0);

	result[3] = vector4<T>(0, 0, 0, 1);

	return result;
}

template <typename T>
quaternion<T> angleAxisQuaternion(T angle, const vector3<T>& axis) {
	T s = std::sin(angle * T(0.5));

	return quaternion<T>(
		axis.x * s,
		axis.y * s,
		axis.z * s,
		std::cos(angle * T(0.5)));
}

template <typename T>
quaternion<T> rotationQuaternion(const vector3<T>& from, const vector3<T>& to) {
	T epsilon = std::numeric_limits<T>::epsilon();

	T cosTheta = dot(from, to);
	if(cosTheta >= T(1) - epsilon) {
		return quaternion<T>();
	}

	if(cosTheta < T(-1) + epsilon) {
		vector3<T> axis = cross(vector3<T>(0, 0, 1), from);
		if(lengthSquared(axis) < epsilon) {
			axis = cross(vector3<T>(1, 0, 0), from);
		}

		return angleAxisQuaternion(T(pi), normalize(axis));
	}

	vector3<T> axis = cross(from, to);

	T s = std::sqrt((T(1) + cosTheta) * T(2));
	T invs = T(1) / s;

	return quaternion<T>(
		axis.x * invs,
		axis.y * invs,
		axis.z * invs,
		s * T(0.5));
}

template <typename T>
matrix3x3<T> lookAtMatrix(const vector3<T>& direction, const vector3<T>& worldUp) {
	vector3<T> forward = normalize(direction);
	vector3<T> right = normalize(cross(forward, worldUp));
	vector3<T> up = cross(right, forward);

	matrix3x3<T> result(T(1));
	result[0][0] = right.x;
	result[1][0] = right.y;
	result[2][0] = right.z;
	result[0][1] = up.x;
	result[1][1] = up.y;
	result[2][1] = up.z;
	result[0][2] = -forward.x;
	result[1][2] = -forward.y;
	result[2][2] = -forward.z;

	return result;
}

template <typename T>
matrix4x4<T> lookAtMatrix(const vector3<T>& eye, const vector3<T>& center, const vector3<T>& worldUp) {
	vector3<T> forward = normalize(center - eye);
	vector3<T> right = normalize(cross(forward, worldUp));
	vector3<T> up = cross(right, forward);

	matrix4x4<T> result(T(1));
	result[0][0] = right.x;
	result[1][0] = right.y;
	result[2][0] = right.z;
	result[0][1] = up.x;
	result[1][1] = up.y;
	result[2][1] = up.z;
	result[0][2] = -forward.x;
	result[1][2] = -forward.y;
	result[2][2] = -forward.z;
	result[3][0] = -dot(right, eye);
	result[3][1] = -dot(up, eye);
	result[3][2] = dot(forward, eye);

	return result;
}

template <typename T>
void extractYawPitchRoll(const matrix4x4<T>& m, T& yaw, T& pitch, T& roll) {
	T t1 = std::atan2(m[2][0], m[2][2]);
	T c2 = std::sqrt(m[0][1] * m[0][1] + m[1][1] * m[1][1]);
	T t2 = std::atan2(-m[2][1], c2);
	T s1 = std::sin(t1);
	T c1 = std::cos(t1);
	T t3 = std::atan2(s1 * m[1][2] - c1 * m[1][0], c1 * m[0][0] - s1 * m[0][2]);

	yaw = t1;
	pitch = t2;
	roll = t3;
}
}
}
