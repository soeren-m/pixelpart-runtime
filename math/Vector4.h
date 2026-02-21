#pragma once

#include "../json/json.hpp"
#include <cstddef>

namespace pixelpart {
namespace math {
template <typename T>
struct vector2;
template <typename T>
struct vector3;

template <typename T>
struct vector4 {
	using ValueType = T;

	T x = T(0);
	T y = T(0);
	T z = T(0);
	T w = T(0);

	constexpr vector4() = default;

	constexpr vector4(T vx, T vy, T vz, T vw) : x(vx), y(vy), z(vz), w(vw) {

	}
	template <typename U1, typename U2, typename U3, typename U4>
	constexpr vector4(U1 vx, U2 vy, U3 vz, U4 vw) :
		x(static_cast<T>(vx)),
		y(static_cast<T>(vy)),
		z(static_cast<T>(vz)),
		w(static_cast<T>(vw)) {

	}

	constexpr explicit vector4(T s) : x(s), y(s), z(s), w(s) {

	}

	constexpr vector4(const vector4<T>& v) = default;
	template <typename U>
	constexpr explicit vector4(const vector4<U>& v) :
		x(static_cast<T>(v.x)),
		y(static_cast<T>(v.y)),
		z(static_cast<T>(v.z)),
		w(static_cast<T>(v.w)) {

	}
	template <typename U1, typename U2, typename U3>
	constexpr vector4(const vector2<U1>& v, U2 vz, U3 vw) :
		x(static_cast<T>(v.x)),
		y(static_cast<T>(v.y)),
		z(static_cast<T>(vz)),
		w(static_cast<T>(vw)) {

	}
	template <typename U1, typename U2>
	constexpr vector4(const vector3<U1>& v, U2 vw) :
		x(static_cast<T>(v.x)),
		y(static_cast<T>(v.y)),
		z(static_cast<T>(v.z)),
		w(static_cast<T>(vw)) {

	}

	vector4<T>& operator=(const vector4<T>& v) = default;

	template <typename U>
	vector4<T>& operator=(const vector4<U>& v) {
		x = static_cast<T>(v.x);
		y = static_cast<T>(v.y);
		z = static_cast<T>(v.z);
		w = static_cast<T>(v.w);

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
			case 3:
				return w;
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
			case 3:
				return w;
		}
	}

	template <typename U>
	vector4<T>& operator+=(const vector4<U>& v) {
		x += static_cast<T>(v.x);
		y += static_cast<T>(v.y);
		z += static_cast<T>(v.z);
		w += static_cast<T>(v.w);

		return *this;
	}
	template <typename U>
	vector4<T>& operator+=(U s) {
		x += static_cast<T>(s);
		y += static_cast<T>(s);
		z += static_cast<T>(s);
		w += static_cast<T>(s);

		return *this;
	}

	template <typename U>
	vector4<T>& operator-=(const vector4<U>& v) {
		x -= static_cast<T>(v.x);
		y -= static_cast<T>(v.y);
		z -= static_cast<T>(v.z);
		w -= static_cast<T>(v.w);

		return *this;
	}
	template <typename U>
	vector4<T>& operator-=(U s) {
		x -= static_cast<T>(s);
		y -= static_cast<T>(s);
		z -= static_cast<T>(s);
		w -= static_cast<T>(s);

		return *this;
	}

	template <typename U>
	vector4<T>& operator*=(const vector4<U>& v) {
		x *= static_cast<T>(v.x);
		y *= static_cast<T>(v.y);
		z *= static_cast<T>(v.z);
		w *= static_cast<T>(v.w);

		return *this;
	}
	template <typename U>
	vector4<T>& operator*=(U s) {
		x *= static_cast<T>(s);
		y *= static_cast<T>(s);
		z *= static_cast<T>(s);
		w *= static_cast<T>(s);

		return *this;
	}

	template <typename U>
	vector4<T>& operator/=(const vector4<U>& v) {
		x /= static_cast<T>(v.x);
		y /= static_cast<T>(v.y);
		z /= static_cast<T>(v.z);
		w /= static_cast<T>(v.w);

		return *this;
	}
	template <typename U>
	vector4<T>& operator/=(U s) {
		x /= static_cast<T>(s);
		y /= static_cast<T>(s);
		z /= static_cast<T>(s);
		w /= static_cast<T>(s);

		return *this;
	}
};

template <typename T>
vector4<T> operator+(const vector4<T>& v) {
	return v;
}
template <typename T>
vector4<T> operator-(const vector4<T>& v) {
	return vector4<T>(
		-v.x,
		-v.y,
		-v.z,
		-v.w);
}

template <typename T>
vector4<T> operator+(const vector4<T>& v1, const vector4<T>& v2) {
	return vector4<T>(
		v1.x + v2.x,
		v1.y + v2.y,
		v1.z + v2.z,
		v1.w + v2.w);
}
template <typename T>
vector4<T> operator+(const vector4<T>& v, T s) {
	return vector4<T>(
		v.x + s,
		v.y + s,
		v.z + s,
		v.w + s);
}
template <typename T>
vector4<T> operator+(T s, const vector4<T>& v) {
	return vector4<T>(
		s + v.x,
		s + v.y,
		s + v.z,
		s + v.w);
}

template <typename T>
vector4<T> operator-(const vector4<T>& v1, const vector4<T>& v2) {
	return vector4<T>(
		v1.x - v2.x,
		v1.y - v2.y,
		v1.z - v2.z,
		v1.w - v2.w);
}
template <typename T>
vector4<T> operator-(const vector4<T>& v, T s) {
	return vector4<T>(
		v.x - s,
		v.y - s,
		v.z - s,
		v.w - s);
}
template <typename T>
vector4<T> operator-(T s, const vector4<T>& v) {
	return vector4<T>(
		s - v.x,
		s - v.y,
		s - v.z,
		s - v.w);
}

template <typename T>
vector4<T> operator*(const vector4<T>& v1, const vector4<T>& v2) {
	return vector4<T>(
		v1.x * v2.x,
		v1.y * v2.y,
		v1.z * v2.z,
		v1.w * v2.w);
}
template <typename T>
vector4<T> operator*(const vector4<T>& v, T s) {
	return vector4<T>(
		v.x * s,
		v.y * s,
		v.z * s,
		v.w * s);
}
template <typename T>
vector4<T> operator*(T s, const vector4<T>& v) {
	return vector4<T>(
		s * v.x,
		s * v.y,
		s * v.z,
		s * v.w);
}

template <typename T>
vector4<T> operator/(const vector4<T>& v1, const vector4<T>& v2) {
	return vector4<T>(
		v1.x / v2.x,
		v1.y / v2.y,
		v1.z / v2.z,
		v1.w / v2.w);
}
template <typename T>
vector4<T> operator/(const vector4<T>& v, T s) {
	return vector4<T>(
		v.x / s,
		v.y / s,
		v.z / s,
		v.w / s);
}
template <typename T>
vector4<T> operator/(T s, const vector4<T>& v) {
	return vector4<T>(
		s / v.x,
		s / v.y,
		s / v.z,
		s / v.w);
}

template <typename T>
bool operator==(const vector4<T>& v1, const vector4<T>& v2) {
	return v1.x == v2.x &&
		v1.y == v2.y &&
		v1.z == v2.z &&
		v1.w == v2.w;
}
template <typename T>
bool operator!=(const vector4<T>& v1, const vector4<T>& v2) {
	return !(v1 == v2);
}

template <typename T>
void to_json(nlohmann::ordered_json& j, const vector4<T>& v) {
	j = nlohmann::ordered_json{
		{ "x", v.x },
		{ "y", v.y },
		{ "z", v.z },
		{ "w", v.w }
	};
}
template <typename T>
void from_json(const nlohmann::ordered_json& j, vector4<T>& v) {
	if(j.is_number()) {
		v.x = j.get<T>();
		v.y = T(0);
		v.z = T(0);
		v.w = T(0);
	}
	else {
		v.x = j.value("x", T(0));
		v.y = j.value("y", T(0));
		v.z = j.value("z", T(0));
		v.w = j.value("w", T(0));
	}
}
}
}
