#pragma once

#include "../json/json.hpp"
#include <cstddef>

namespace pixelpart {
namespace math {
template <typename T>
struct vector2;
template <typename T>
struct vector4;

template <typename T>
struct vector3 {
	using ValueType = T;

	T x = T(0);
	T y = T(0);
	T z = T(0);

	constexpr vector3() = default;

	constexpr vector3(T vx, T vy, T vz) : x(vx), y(vy), z(vz) {

	}
	template <typename U1, typename U2, typename U3>
	constexpr vector3(U1 vx, U2 vy, U3 vz) :
		x(static_cast<T>(vx)),
		y(static_cast<T>(vy)),
		z(static_cast<T>(vz)) {

	}

	constexpr explicit vector3(T s) : x(s), y(s), z(s) {

	}

	constexpr vector3(const vector3<T>& v) = default;
	template <typename U>
	constexpr explicit vector3(const vector3<U>& v) :
		x(static_cast<T>(v.x)),
		y(static_cast<T>(v.y)),
		z(static_cast<T>(v.z)) {

	}
	template <typename U1, typename U2>
	constexpr vector3(const vector2<U1>& v, U2 vz) :
		x(static_cast<T>(v.x)),
		y(static_cast<T>(v.y)),
		z(static_cast<T>(vz)) {

	}
	template <typename U>
	constexpr explicit vector3(const vector4<U>& v) :
		x(static_cast<T>(v.x)),
		y(static_cast<T>(v.y)),
		z(static_cast<T>(v.z)) {

	}

	vector3<T>& operator=(const vector3<T>& v) = default;

	template <typename U>
	vector3<T>& operator=(const vector3<U>& v) {
		x = static_cast<T>(v.x);
		y = static_cast<T>(v.y);
		z = static_cast<T>(v.z);

		return *this;
	}

	T& operator[](std::size_t index) {
		switch(index) {
			default:
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
		}
	}
	const T& operator[](std::size_t index) const {
		switch(index) {
			default:
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
		}
	}

	template <typename U>
	vector3<T>& operator+=(const vector3<U>& v) {
		x += static_cast<T>(v.x);
		y += static_cast<T>(v.y);
		z += static_cast<T>(v.z);

		return *this;
	}
	template <typename U>
	vector3<T>& operator+=(U s) {
		x += static_cast<T>(s);
		y += static_cast<T>(s);
		z += static_cast<T>(s);

		return *this;
	}

	template <typename U>
	vector3<T>& operator-=(const vector3<U>& v) {
		x -= static_cast<T>(v.x);
		y -= static_cast<T>(v.y);
		z -= static_cast<T>(v.z);

		return *this;
	}
	template <typename U>
	vector3<T>& operator-=(U s) {
		x -= static_cast<T>(s);
		y -= static_cast<T>(s);
		z -= static_cast<T>(s);

		return *this;
	}

	template <typename U>
	vector3<T>& operator*=(const vector3<U>& v) {
		x *= static_cast<T>(v.x);
		y *= static_cast<T>(v.y);
		z *= static_cast<T>(v.z);

		return *this;
	}
	template <typename U>
	vector3<T>& operator*=(U s) {
		x *= static_cast<T>(s);
		y *= static_cast<T>(s);
		z *= static_cast<T>(s);

		return *this;
	}

	template <typename U>
	vector3<T>& operator/=(const vector3<U>& v) {
		x /= static_cast<T>(v.x);
		y /= static_cast<T>(v.y);
		z /= static_cast<T>(v.z);

		return *this;
	}
	template <typename U>
	vector3<T>& operator/=(U s) {
		x /= static_cast<T>(s);
		y /= static_cast<T>(s);
		z /= static_cast<T>(s);

		return *this;
	}
};

template <typename T>
vector3<T> operator+(const vector3<T>& v) {
	return v;
}
template <typename T>
vector3<T> operator-(const vector3<T>& v) {
	return vector3<T>(
		-v.x,
		-v.y,
		-v.z);
}

template <typename T>
vector3<T> operator+(const vector3<T>& v1, const vector3<T>& v2) {
	return vector3<T>(
		v1.x + v2.x,
		v1.y + v2.y,
		v1.z + v2.z);
}
template <typename T>
vector3<T> operator+(const vector3<T>& v, T s) {
	return vector3<T>(
		v.x + s,
		v.y + s,
		v.z + s);
}
template <typename T>
vector3<T> operator+(T s, const vector3<T>& v) {
	return vector3<T>(
		s + v.x,
		s + v.y,
		s + v.z);
}

template <typename T>
vector3<T> operator-(const vector3<T>& v1, const vector3<T>& v2) {
	return vector3<T>(
		v1.x - v2.x,
		v1.y - v2.y,
		v1.z - v2.z);
}
template <typename T>
vector3<T> operator-(const vector3<T>& v, T s) {
	return vector3<T>(
		v.x - s,
		v.y - s,
		v.z - s);
}
template <typename T>
vector3<T> operator-(T s, const vector3<T>& v) {
	return vector3<T>(
		s - v.x,
		s - v.y,
		s - v.z);
}

template <typename T>
vector3<T> operator*(const vector3<T>& v1, const vector3<T>& v2) {
	return vector3<T>(
		v1.x * v2.x,
		v1.y * v2.y,
		v1.z * v2.z);
}
template <typename T>
vector3<T> operator*(const vector3<T>& v, T s) {
	return vector3<T>(
		v.x * s,
		v.y * s,
		v.z * s);
}
template <typename T>
vector3<T> operator*(T s, const vector3<T>& v) {
	return vector3<T>(
		s * v.x,
		s * v.y,
		s * v.z);
}

template <typename T>
vector3<T> operator/(const vector3<T>& v1, const vector3<T>& v2) {
	return vector3<T>(
		v1.x / v2.x,
		v1.y / v2.y,
		v1.z / v2.z);
}
template <typename T>
vector3<T> operator/(const vector3<T>& v, T s) {
	return vector3<T>(
		v.x / s,
		v.y / s,
		v.z / s);
}
template <typename T>
vector3<T> operator/(T s, const vector3<T>& v) {
	return vector3<T>(
		s / v.x,
		s / v.y,
		s / v.z);
}

template <typename T>
bool operator==(const vector3<T>& v1, const vector3<T>& v2) {
	return v1.x == v2.x &&
		v1.y == v2.y &&
		v1.z == v2.z;
}
template <typename T>
bool operator!=(const vector3<T>& v1, const vector3<T>& v2) {
	return !(v1 == v2);
}

template <typename T>
void to_json(nlohmann::ordered_json& j, const vector3<T>& v) {
	j = nlohmann::ordered_json{
		{ "x", v.x },
		{ "y", v.y },
		{ "z", v.z }
	};
}
template <typename T>
void from_json(const nlohmann::ordered_json& j, vector3<T>& v) {
	if(j.is_number()) {
		v.x = j.get<T>();
		v.y = T(0);
		v.z = T(0);
	}
	else {
		v.x = j.value("x", T(0));
		v.y = j.value("y", T(0));
		v.z = j.value("z", T(0));
	}
}
}
}
