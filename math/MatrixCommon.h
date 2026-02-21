#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix2x2.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"

namespace pixelpart {
namespace math {
template <typename T>
matrix2x2<T> outerProduct(const vector2<T>& column, const vector2<T>& row) {
	matrix2x2<T> result;
	result[0] = column * row[0];
	result[1] = column * row[1];

	return result;
}
template <typename T>
matrix3x3<T> outerProduct(const vector3<T>& column, const vector3<T>& row) {
	matrix3x3<T> result;
	result[0] = column * row[0];
	result[1] = column * row[1];
	result[2] = column * row[2];

	return result;
}
template <typename T>
matrix4x4<T> outerProduct(const vector4<T>& column, const vector4<T>& row) {
	matrix4x4<T> result;
	result[0] = column * row[0];
	result[1] = column * row[1];
	result[2] = column * row[2];
	result[3] = column * row[3];

	return result;
}

template <typename T>
T determinant(const matrix2x2<T>& m) {
	return m[0][0] * m[1][1] - m[1][0] * m[0][1];
}
template <typename T>
T determinant(const matrix3x3<T>& m) {
	return m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
		- m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
		+ m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);
}
template <typename T>
T determinant(const matrix4x4<T>& m) {
	T subFactor00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
	T subFactor01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
	T subFactor02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
	T subFactor03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
	T subFactor04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
	T subFactor05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

	vector4<T> detCof(
		(m[1][1] * subFactor00 - m[1][2] * subFactor01 + m[1][3] * subFactor02),
		-(m[1][0] * subFactor00 - m[1][2] * subFactor03 + m[1][3] * subFactor04),
		(m[1][0] * subFactor01 - m[1][1] * subFactor03 + m[1][3] * subFactor05),
		-(m[1][0] * subFactor02 - m[1][1] * subFactor04 + m[1][2] * subFactor05));

	return
		m[0][0] * detCof[0] + m[0][1] * detCof[1] +
		m[0][2] * detCof[2] + m[0][3] * detCof[3];
}

template <typename T>
matrix2x2<T> transpose(const matrix2x2<T>& m) {
	matrix2x2<T> result;
	result[0][0] = m[0][0];
	result[0][1] = m[1][0];

	result[1][0] = m[0][1];
	result[1][1] = m[1][1];

	return result;
}
template <typename T>
matrix3x3<T> transpose(const matrix3x3<T>& m) {
	matrix3x3<T> result;
	result[0][0] = m[0][0];
	result[0][1] = m[1][0];
	result[0][2] = m[2][0];

	result[1][0] = m[0][1];
	result[1][1] = m[1][1];
	result[1][2] = m[2][1];

	result[2][0] = m[0][2];
	result[2][1] = m[1][2];
	result[2][2] = m[2][2];

	return result;
}
template <typename T>
matrix4x4<T> transpose(const matrix4x4<T>& m) {
	matrix4x4<T> result;
	result[0][0] = m[0][0];
	result[0][1] = m[1][0];
	result[0][2] = m[2][0];
	result[0][3] = m[3][0];

	result[1][0] = m[0][1];
	result[1][1] = m[1][1];
	result[1][2] = m[2][1];
	result[1][3] = m[3][1];

	result[2][0] = m[0][2];
	result[2][1] = m[1][2];
	result[2][2] = m[2][2];
	result[2][3] = m[3][2];

	result[3][0] = m[0][3];
	result[3][1] = m[1][3];
	result[3][2] = m[2][3];
	result[3][3] = m[3][3];

	return result;
}

template <typename T>
matrix2x2<T> inverse(const matrix2x2<T>& m) {
	T oneOverDeterminant = T(1) / determinant(m);

	matrix2x2<T> result(
		m[1][1] * oneOverDeterminant,
		-m[0][1] * oneOverDeterminant,
		-m[1][0] * oneOverDeterminant,
		m[0][0] * oneOverDeterminant);

	return result;
}
template <typename T>
matrix3x3<T> inverse(const matrix3x3<T>& m) {
	T oneOverDeterminant = T(1) / determinant(m);

	matrix3x3<T> result;
	result[0][0] = +(m[1][1] * m[2][2] - m[2][1] * m[1][2]);
	result[1][0] = -(m[1][0] * m[2][2] - m[2][0] * m[1][2]);
	result[2][0] = +(m[1][0] * m[2][1] - m[2][0] * m[1][1]);
	result[0][1] = -(m[0][1] * m[2][2] - m[2][1] * m[0][2]);
	result[1][1] = +(m[0][0] * m[2][2] - m[2][0] * m[0][2]);
	result[2][1] = -(m[0][0] * m[2][1] - m[2][0] * m[0][1]);
	result[0][2] = +(m[0][1] * m[1][2] - m[1][1] * m[0][2]);
	result[1][2] = -(m[0][0] * m[1][2] - m[1][0] * m[0][2]);
	result[2][2] = +(m[0][0] * m[1][1] - m[1][0] * m[0][1]);

	result *= oneOverDeterminant;

	return result;
}
template <typename T>
matrix4x4<T> inverse(const matrix4x4<T>& m) {
	T coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
	T coef02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
	T coef03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

	T coef04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
	T coef06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
	T coef07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

	T coef08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
	T coef10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
	T coef11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

	T coef12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
	T coef14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
	T coef15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

	T coef16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
	T coef18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
	T coef19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

	T coef20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
	T coef22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
	T coef23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

	vector4<T> fac0(coef00, coef00, coef02, coef03);
	vector4<T> fac1(coef04, coef04, coef06, coef07);
	vector4<T> fac2(coef08, coef08, coef10, coef11);
	vector4<T> fac3(coef12, coef12, coef14, coef15);
	vector4<T> fac4(coef16, coef16, coef18, coef19);
	vector4<T> fac5(coef20, coef20, coef22, coef23);

	vector4<T> vec0(m[1][0], m[0][0], m[0][0], m[0][0]);
	vector4<T> vec1(m[1][1], m[0][1], m[0][1], m[0][1]);
	vector4<T> vec2(m[1][2], m[0][2], m[0][2], m[0][2]);
	vector4<T> vec3(m[1][3], m[0][3], m[0][3], m[0][3]);

	vector4<T> inv0(vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
	vector4<T> inv1(vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
	vector4<T> inv2(vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
	vector4<T> inv3(vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

	vector4<T> signA(+1, -1, +1, -1);
	vector4<T> signB(-1, +1, -1, +1);
	matrix4x4<T> result(inv0 * signA, inv1 * signB, inv2 * signA, inv3 * signB);

	vector4<T> row0(result[0][0], result[1][0], result[2][0], result[3][0]);
	vector4<T> dot0(m[0] * row0);
	T dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);

	T oneOverDeterminant = T(1) / dot1;

	return result * oneOverDeterminant;
}
}
}
