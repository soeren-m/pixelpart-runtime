#pragma once

#include "Vector4.h"
#include "../json/json.hpp"

namespace pixelpart {
namespace math {
template <typename T>
struct matrix2x2;
template <typename T>
struct matrix3x3;

template <typename T>
struct matrix4x4 {
	vector4<T> columns[4];

	constexpr matrix4x4() = default;

	constexpr matrix4x4(const vector4<T>& v1, const vector4<T>& v2, const vector4<T>& v3, const vector4<T>& v4) :
		columns{ v1, v2, v3, v4 } {

	}
	template <typename U1, typename U2, typename U3, typename U4>
	constexpr matrix4x4(const vector4<U1>& v1, const vector4<U2>& v2, const vector4<U3>& v3, const vector4<U4>& v4) :
		columns{ vector4<T>(v1), vector4<T>(v2), vector4<T>(v3), vector4<T>(v4) } {

	}

	constexpr explicit matrix4x4(T s) :
		columns{ vector4<T>(s, 0, 0, 0), vector4<T>(0, s, 0, 0), vector4<T>(0, 0, s, 0), vector4<T>(0, 0, 0, s) } {

	}

	constexpr matrix4x4(const matrix4x4<T>& m) = default;
	template <typename U>
	constexpr explicit matrix4x4(const matrix4x4<U>& m) :
		columns{ vector4<T>(m[0]), vector4<T>(m[1]), vector4<T>(m[2]), vector4<T>(m[3]) } {

	}
	constexpr explicit matrix4x4(const matrix2x2<T>& m) :
		columns{ vector4<T>(m[0], 0, 0), vector4<T>(m[1], 0, 0), vector4<T>(0, 0, 1, 0), vector4<T>(0, 0, 0, 1) } {

	}

	constexpr explicit matrix4x4(const matrix3x3<T>& m) :
		columns{ vector4<T>(m[0], 0), vector4<T>(m[1], 0), vector4<T>(m[2], 0), vector4<T>(0, 0, 0, 1) } {

	}

	template <typename U>
	matrix4x4<T>& operator=(const matrix4x4<U>& m) {
		columns[0] = m.columns[0];
		columns[1] = m.columns[1];
		columns[2] = m.columns[2];
		columns[3] = m.columns[3];

		return *this;
	}

	vector4<T>& operator[](std::size_t index) {
		return columns[index];
	}
	const vector4<T>& operator[](std::size_t index) const {
		return columns[index];
	}

	template <typename U>
	matrix4x4<T>& operator+=(const matrix4x4<U>& m) {
		columns[0] += m.columns[0];
		columns[1] += m.columns[1];
		columns[2] += m.columns[2];
		columns[3] += m.columns[3];

		return *this;
	}
	template <typename U>
	matrix4x4<T>& operator+=(U s) {
		columns[0] += s;
		columns[1] += s;
		columns[2] += s;
		columns[3] += s;

		return *this;
	}

	template <typename U>
	matrix4x4<T>& operator-=(const matrix4x4<U>& m) {
		columns[0] -= m.columns[0];
		columns[1] -= m.columns[1];
		columns[2] -= m.columns[2];
		columns[3] -= m.columns[3];

		return *this;
	}
	template <typename U>
	matrix4x4<T>& operator-=(U s) {
		columns[0] -= s;
		columns[1] -= s;
		columns[2] -= s;
		columns[3] -= s;

		return *this;
	}

	template <typename U>
	matrix4x4<T>& operator*=(const matrix4x4<U>& m) {
		return (*this = *this * m);
	}
	template <typename U>
	matrix4x4<T>& operator*=(U s) {
		columns[0] *= s;
		columns[1] *= s;
		columns[2] *= s;
		columns[3] *= s;

		return *this;
	}

	template <typename U>
	matrix4x4<T>& operator/=(U s) {
		columns[0] /= s;
		columns[1] /= s;
		columns[2] /= s;
		columns[3] /= s;

		return *this;
	}
};

template <typename T>
matrix4x4<T> operator+(const matrix4x4<T>& m) {
	return m;
}
template <typename T>
matrix4x4<T> operator-(const matrix4x4<T>& m) {
	return matrix4x4<T>(
		-m[0],
		-m[1],
		-m[2],
		-m[3]);
}

template <typename T>
matrix4x4<T> operator+(const matrix4x4<T>& m1, const matrix4x4<T>& m2) {
	return matrix4x4<T>(
		m1[0] + m2[0],
		m1[1] + m2[1],
		m1[2] + m2[2],
		m1[3] + m2[3]);
}
template <typename T>
matrix4x4<T> operator+(const matrix4x4<T>& m, T s) {
	return matrix4x4<T>(
		m[0] + s,
		m[1] + s,
		m[2] + s,
		m[3] + s);
}

template <typename T>
matrix4x4<T> operator-(const matrix4x4<T>& m1, const matrix4x4<T>& m2) {
	return matrix4x4<T>(
		m1[0] - m2[0],
		m1[1] - m2[1],
		m1[2] - m2[2],
		m1[3] - m2[3]);
}
template <typename T>
matrix4x4<T> operator-(const matrix4x4<T>& m, T s) {
	return matrix4x4<T>(
		m[0] - s,
		m[1] - s,
		m[2] - s,
		m[3] - s);
}

template <typename T>
matrix4x4<T> operator*(const matrix4x4<T>& m1, const matrix4x4<T>& m2) {
	return matrix4x4<T>(
		m1[0] * m2[0].x + m1[1] * m2[0].y + m1[2] * m2[0].z + m1[3] * m2[0].w,
		m1[0] * m2[1].x + m1[1] * m2[1].y + m1[2] * m2[1].z + m1[3] * m2[1].w,
		m1[0] * m2[2].x + m1[1] * m2[2].y + m1[2] * m2[2].z + m1[3] * m2[2].w,
		m1[0] * m2[3].x + m1[1] * m2[3].y + m1[2] * m2[3].z + m1[3] * m2[3].w);
}
template <typename T>
vector4<T> operator*(const matrix4x4<T>& m, const vector4<T>& v) {
	return vector4<T>(
		m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w,
		m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w,
		m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w,
		m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w);
}
template <typename T>
vector4<T> operator*(const vector4<T>& v, const matrix4x4<T>& m) {
	return vector4<T>(
		v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + v.w * m[0][3],
		v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + v.w * m[1][3],
		v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2] + v.w * m[2][3],
		v.x * m[3][0] + v.y * m[3][1] + v.z * m[3][2] + v.w * m[3][3]);
}
template <typename T>
matrix4x4<T> operator*(const matrix4x4<T>& m, T s) {
	return matrix4x4<T>(
		m[0] * s,
		m[1] * s,
		m[2] * s,
		m[3] * s);
}

template <typename T>
matrix4x4<T> operator/(const matrix4x4<T>& m, T s) {
	return matrix4x4<T>(
		m[0] / s,
		m[1] / s,
		m[2] / s,
		m[3] / s);
}

template <typename T>
bool operator==(const matrix4x4<T>& m1, const matrix4x4<T>& m2) {
	return m1[0] == m2[0] &&
		m1[1] == m2[1] &&
		m1[2] == m2[2] &&
		m1[3] == m2[3];
}
template <typename T>
bool operator!=(const matrix4x4<T>& m1, const matrix4x4<T>& m2) {
	return !(m1 == m2);
}

template <typename T>
void to_json(nlohmann::ordered_json& j, const matrix4x4<T>& m) {
	j = nlohmann::ordered_json::array({
		m[0],
		m[1],
		m[2],
		m[3]
	});
}
template <typename T>
void from_json(const nlohmann::ordered_json& j, matrix4x4<T>& m) {
	m = matrix4x4<T>();
	if(!j.is_array()) {
		return;
	}

	if(j.size() > 0) {
		m[0] = j[0].get<vector4<T>>();
	}
	if(j.size() > 1) {
		m[1] = j[1].get<vector4<T>>();
	}
	if(j.size() > 2) {
		m[2] = j[2].get<vector4<T>>();
	}
	if(j.size() > 3) {
		m[3] = j[3].get<vector4<T>>();
	}
}
}
}
