#pragma once

#include "Vector2.h"
#include "../json/json.hpp"

namespace pixelpart {
namespace math {
template <typename T>
struct matrix3x3;
template <typename T>
struct matrix4x4;

template <typename T>
struct matrix2x2 {
	vector2<T> columns[2];

	constexpr matrix2x2() = default;

	constexpr matrix2x2(const vector2<T>& v1, const vector2<T>& v2) :
		columns{ v1, v2 } {

	}
	template <typename U1, typename U2>
	constexpr matrix2x2(const vector2<U1>& v1, const vector2<U2>& v2) :
		columns{ vector2<T>(v1), vector2<T>(v2) } {

	}

	constexpr explicit matrix2x2(T s) :
		columns{ vector2<T>(s, 0), vector2<T>(0, s) } {

	}

	constexpr matrix2x2(const matrix2x2<T>& m) = default;
	template <typename U>
	constexpr explicit matrix2x2(const matrix2x2<U>& m) :
		columns{ vector2<T>(m[0]), vector2<T>(m[1]) } {

	}
	constexpr explicit matrix2x2(const matrix3x3<T>& m) :
		columns{ vector2<T>(m[0]), vector2<T>(m[1]) } {

	}
	constexpr explicit matrix2x2(const matrix4x4<T>& m) :
		columns{ vector2<T>(m[0]), vector2<T>(m[1]) } {

	}

	template <typename U>
	matrix2x2<T>& operator=(const matrix2x2<U>& m) {
		columns[0] = m.columns[0];
		columns[1] = m.columns[1];

		return *this;
	}

	vector2<T>& operator[](std::size_t index) {
		return columns[index];
	}
	const vector2<T>& operator[](std::size_t index) const {
		return columns[index];
	}

	template <typename U>
	matrix2x2<T>& operator+=(const matrix2x2<U>& m) {
		columns[0] += m.columns[0];
		columns[1] += m.columns[1];

		return *this;
	}
	template <typename U>
	matrix2x2<T>& operator+=(U s) {
		columns[0] += s;
		columns[1] += s;

		return *this;
	}

	template <typename U>
	matrix2x2<T>& operator-=(const matrix2x2<U>& m) {
		columns[0] -= m.columns[0];
		columns[1] -= m.columns[1];

		return *this;
	}
	template <typename U>
	matrix2x2<T>& operator-=(U s) {
		columns[0] -= s;
		columns[1] -= s;

		return *this;
	}

	template <typename U>
	matrix2x2<T>& operator*=(const matrix2x2<U>& m) {
		return (*this = *this * m);
	}
	template <typename U>
	matrix2x2<T>& operator*=(U s) {
		columns[0] *= s;
		columns[1] *= s;

		return *this;
	}

	template <typename U>
	matrix2x2<T>& operator/=(U s) {
		columns[0] /= s;
		columns[1] /= s;

		return *this;
	}
};

template <typename T>
matrix2x2<T> operator+(const matrix2x2<T>& m) {
	return m;
}
template <typename T>
matrix2x2<T> operator-(const matrix2x2<T>& m) {
	return matrix2x2<T>(
		-m[0],
		-m[1]);
}

template <typename T>
matrix2x2<T> operator+(const matrix2x2<T>& m1, const matrix2x2<T>& m2) {
	return matrix2x2<T>(
		m1[0] + m2[0],
		m1[1] + m2[1]);
}
template <typename T>
matrix2x2<T> operator+(const matrix2x2<T>& m, T s) {
	return matrix2x2<T>(
		m[0] + s,
		m[1] + s);
}

template <typename T>
matrix2x2<T> operator-(const matrix2x2<T>& m1, const matrix2x2<T>& m2) {
	return matrix2x2<T>(
		m1[0] - m2[0],
		m1[1] - m2[1]);
}
template <typename T>
matrix2x2<T> operator-(const matrix2x2<T>& m, T s) {
	return matrix2x2<T>(
		m[0] - s,
		m[1] - s);
}

template <typename T>
matrix2x2<T> operator*(const matrix2x2<T>& m1, const matrix2x2<T>& m2) {
	return matrix2x2<T>(
		m1[0] * m2[0].x + m1[1] * m2[0].y,
		m1[0] * m2[1].x + m1[1] * m2[1].y);
}
template <typename T>
vector2<T> operator*(const matrix2x2<T>& m, const vector2<T>& v) {
	return vector2<T>(
		m[0][0] * v.x + m[1][0] * v.y,
		m[0][1] * v.x + m[1][1] * v.y);
}
template <typename T>
vector2<T> operator*(const vector2<T>& v, const matrix2x2<T>& m) {
	return vector2<T>(
		v.x * m[0][0] + v.y * m[0][1],
		v.x * m[1][0] + v.y * m[1][1]);
}
template <typename T>
matrix2x2<T> operator*(const matrix2x2<T>& m, T s) {
	return matrix2x2<T>(
		m[0] * s,
		m[1] * s);
}

template <typename T>
matrix2x2<T> operator/(const matrix2x2<T>& m, T s) {
	return matrix2x2<T>(
		m[0] / s,
		m[1] / s);
}

template <typename T>
bool operator==(const matrix2x2<T>& m1, const matrix2x2<T>& m2) {
	return m1[0] == m2[0] &&
		m1[1] == m2[1];
}
template <typename T>
bool operator!=(const matrix2x2<T>& m1, const matrix2x2<T>& m2) {
	return !(m1 == m2);
}

template <typename T>
void to_json(nlohmann::ordered_json& j, const matrix2x2<T>& m) {
	j = nlohmann::ordered_json::array({
		m[0],
		m[1]
	});
}
template <typename T>
void from_json(const nlohmann::ordered_json& j, matrix2x2<T>& m) {
	m = matrix2x2<T>();
	if(!j.is_array()) {
		return;
	}

	if(j.size() > 0) {
		m[0] = j[0].get<vector2<T>>();
	}
	if(j.size() > 1) {
		m[1] = j[1].get<vector2<T>>();
	}
}
}
}
