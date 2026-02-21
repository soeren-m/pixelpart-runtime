#pragma once

#include "../json/json.hpp"
#include <cstddef>

namespace pixelpart {
namespace math {
template <typename T>
struct vector3;
template <typename T>
struct vector4;

template <typename T>
struct vector2 {
	using ValueType = T;

	T x = T(0);
	T y = T(0);

	constexpr vector2() = default;

	constexpr vector2(T vx, T vy) : x(vx), y(vy) {

	}
	template <typename U1, typename U2>
	constexpr vector2(U1 vx, U2 vy) :
		x(static_cast<T>(vx)),
		y(static_cast<T>(vy)) {

	}

	constexpr explicit vector2(T s) : x(s), y(s) {

	}

	constexpr vector2(const vector2<T>& v) = default;
	template <typename U>
	constexpr explicit vector2(const vector2<U>& v) :
		x(static_cast<T>(v.x)),
		y(static_cast<T>(v.y)) {

	}
	template <typename U>
	constexpr explicit vector2(const vector3<U>& v) :
		x(static_cast<T>(v.x)),
		y(static_cast<T>(v.y)) {

	}
	template <typename U>
	constexpr explicit vector2(const vector4<U>& v) :
		x(static_cast<T>(v.x)),
		y(static_cast<T>(v.y)) {

	}

	vector2<T>& operator=(const vector2<T>& v) = default;

	template <typename U>
	vector2<T>& operator=(const vector2<U>& v) {
		x = static_cast<T>(v.x);
		y = static_cast<T>(v.y);

		return *this;
	}

	T& operator[](std::size_t index) {
		switch(index) {
			default:
			case 0:
				return x;
			case 1:
				return y;
		}
	}
	const T& operator[](std::size_t index) const {
		switch(index) {
			default:
			case 0:
				return x;
			case 1:
				return y;
		}
	}

	template <typename U>
	vector2<T>& operator+=(const vector2<U>& v) {
		x += static_cast<T>(v.x);
		y += static_cast<T>(v.y);

		return *this;
	}
	template <typename U>
	vector2<T>& operator+=(U s) {
		x += static_cast<T>(s);
		y += static_cast<T>(s);

		return *this;
	}

	template <typename U>
	vector2<T>& operator-=(const vector2<U>& v) {
		x -= static_cast<T>(v.x);
		y -= static_cast<T>(v.y);

		return *this;
	}
	template <typename U>
	vector2<T>& operator-=(U s) {
		x -= static_cast<T>(s);
		y -= static_cast<T>(s);

		return *this;
	}

	template <typename U>
	vector2<T>& operator*=(const vector2<U>& v) {
		x *= static_cast<T>(v.x);
		y *= static_cast<T>(v.y);

		return *this;
	}
	template <typename U>
	vector2<T>& operator*=(U s) {
		x *= static_cast<T>(s);
		y *= static_cast<T>(s);

		return *this;
	}

	template <typename U>
	vector2<T>& operator/=(const vector2<U>& v) {
		x /= static_cast<T>(v.x);
		y /= static_cast<T>(v.y);

		return *this;
	}
	template <typename U>
	vector2<T>& operator/=(U s) {
		x /= static_cast<T>(s);
		y /= static_cast<T>(s);

		return *this;
	}
};

template <typename T>
vector2<T> operator+(const vector2<T>& v) {
	return v;
}
template <typename T>
vector2<T> operator-(const vector2<T>& v) {
	return vector2<T>(
		-v.x,
		-v.y);
}

template <typename T>
vector2<T> operator+(const vector2<T>& v1, const vector2<T>& v2) {
	return vector2<T>(
		v1.x + v2.x,
		v1.y + v2.y);
}
template <typename T>
vector2<T> operator+(const vector2<T>& v, T s) {
	return vector2<T>(
		v.x + s,
		v.y + s);
}
template <typename T>
vector2<T> operator+(T s, const vector2<T>& v) {
	return vector2<T>(
		s + v.x,
		s + v.y);
}

template <typename T>
vector2<T> operator-(const vector2<T>& v1, const vector2<T>& v2) {
	return vector2<T>(
		v1.x - v2.x,
		v1.y - v2.y);
}
template <typename T>
vector2<T> operator-(const vector2<T>& v, T s) {
	return vector2<T>(
		v.x - s,
		v.y - s);
}
template <typename T>
vector2<T> operator-(T s, const vector2<T>& v) {
	return vector2<T>(
		s - v.x,
		s - v.y);
}

template <typename T>
vector2<T> operator*(const vector2<T>& v1, const vector2<T>& v2) {
	return vector2<T>(
		v1.x * v2.x,
		v1.y * v2.y);
}
template <typename T>
vector2<T> operator*(const vector2<T>& v, T s) {
	return vector2<T>(
		v.x * s,
		v.y * s);
}
template <typename T>
vector2<T> operator*(T s, const vector2<T>& v) {
	return vector2<T>(
		s * v.x,
		s * v.y);
}

template <typename T>
vector2<T> operator/(const vector2<T>& v1, const vector2<T>& v2) {
	return vector2<T>(
		v1.x / v2.x,
		v1.y / v2.y);
}
template <typename T>
vector2<T> operator/(const vector2<T>& v, T s) {
	return vector2<T>(
		v.x / s,
		v.y / s);
}
template <typename T>
vector2<T> operator/(T s, const vector2<T>& v) {
	return vector2<T>(
		s / v.x,
		s / v.y);
}

template <typename T>
bool operator==(const vector2<T>& v1, const vector2<T>& v2) {
	return v1.x == v2.x &&
		v1.y == v2.y;
}
template <typename T>
bool operator!=(const vector2<T>& v1, const vector2<T>& v2) {
	return !(v1 == v2);
}

template <typename T>
void to_json(nlohmann::ordered_json& j, const vector2<T>& v) {
	j = nlohmann::ordered_json{
		{ "x", v.x },
		{ "y", v.y }
	};
}
template <typename T>
void from_json(const nlohmann::ordered_json& j, vector2<T>& v) {
	if(j.is_number()) {
		v.x = j.get<T>();
		v.y = T(0);
	}
	else {
		v.x = j.value("x", T(0));
		v.y = j.value("y", T(0));
	}
}
}
}
