#pragma once

#include "../json/json.hpp"
#include <cstddef>

namespace pixelpart {
namespace math {
template <typename T>
struct quaternion {
	using ValueType = T;

	T x = T(0);
	T y = T(0);
	T z = T(0);
	T w = T(1);

	constexpr quaternion() = default;

	constexpr quaternion(T qx, T qy, T qz, T qw) : x(qx), y(qy), z(qz), w(qw) {

	}
	template <typename U1, typename U2, typename U3, typename U4>
	constexpr quaternion(U1 qx, U2 qy, U3 qz, U4 qw) :
		x(static_cast<T>(qx)),
		y(static_cast<T>(qy)),
		z(static_cast<T>(qz)),
		w(static_cast<T>(qw)) {

	}

	constexpr quaternion(const quaternion<T>& q) = default;
	template <typename U>
	constexpr explicit quaternion(const quaternion<U>& q) :
		x(static_cast<T>(q.x)),
		y(static_cast<T>(q.y)),
		z(static_cast<T>(q.z)),
		w(static_cast<T>(q.w)) {

	}

	quaternion<T>& operator=(const quaternion<T>& q) = default;

	template <typename U>
	quaternion<T>& operator=(const quaternion<U>& q) {
		x = static_cast<T>(q.x);
		y = static_cast<T>(q.y);
		z = static_cast<T>(q.z);
		w = static_cast<T>(q.w);

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
	quaternion<T>& operator+=(const quaternion<U>& q) {
		x += static_cast<T>(q.x);
		y += static_cast<T>(q.y);
		z += static_cast<T>(q.z);
		w += static_cast<T>(q.w);

		return *this;
	}

	template <typename U>
	quaternion<T>& operator-=(const quaternion<U>& q) {
		x -= static_cast<T>(q.x);
		y -= static_cast<T>(q.y);
		z -= static_cast<T>(q.z);
		w -= static_cast<T>(q.w);

		return *this;
	}

	template <typename U>
	quaternion<T>& operator*=(const quaternion<U>& q) {
		const quaternion<T> l(*this);
		const quaternion<T> r(q);
		x = l.w * r.x + l.x * r.w + l.y * r.z - l.z * r.y;
		y = l.w * r.y + l.y * r.w + l.z * r.x - l.x * r.z;
		z = l.w * r.z + l.z * r.w + l.x * r.y - l.y * r.x;
		w = l.w * r.w - l.x * r.x - l.y * r.y - l.z * r.z;

		return *this;
	}
	template <typename U>
	quaternion<T>& operator*=(U s) {
		x *= static_cast<T>(s);
		y *= static_cast<T>(s);
		z *= static_cast<T>(s);
		w *= static_cast<T>(s);

		return *this;
	}

	template <typename U>
	quaternion<T>& operator/=(U s) {
		x /= static_cast<T>(s);
		y /= static_cast<T>(s);
		z /= static_cast<T>(s);
		w /= static_cast<T>(s);

		return *this;
	}
};

template <typename T>
quaternion<T> operator+(const quaternion<T>& q) {
	return q;
}
template <typename T>
quaternion<T> operator-(const quaternion<T>& q) {
	return quaternion<T>(
		-q.x,
		-q.y,
		-q.z,
		-q.w);
}

template <typename T>
quaternion<T> operator+(const quaternion<T>& q1, const quaternion<T>& q2) {
	return quaternion<T>(q1) += q2;
}

template <typename T>
quaternion<T> operator-(const quaternion<T>& q1, const quaternion<T>& q2) {
	return quaternion<T>(q1) -= q2;
}

template <typename T>
quaternion<T> operator*(const quaternion<T>& q1, const quaternion<T>& q2) {
	return quaternion<T>(q1) *= q2;
}
template <typename T>
quaternion<T> operator*(const quaternion<T>& q, T s) {
	return quaternion<T>(q) *= s;
}
template <typename T>
quaternion<T> operator*(T s, const quaternion<T>& q) {
	return q * s;
}

template <typename T>
quaternion<T> operator/(const quaternion<T>& q, T s) {
	return quaternion<T>(q) /= s;
}

template <typename T>
bool operator==(const quaternion<T>& q1, const quaternion<T>& q2) {
	return q1.x == q2.x &&
		q1.y == q2.y &&
		q1.z == q2.z &&
		q1.w == q2.w;
}
template <typename T>
bool operator!=(const quaternion<T>& q1, const quaternion<T>& q2) {
	return !(q1 == q2);
}

template <typename T>
void to_json(nlohmann::ordered_json& j, const quaternion<T>& q) {
	j = nlohmann::ordered_json{
		{ "x", q.x },
		{ "y", q.y },
		{ "z", q.z },
		{ "w", q.w }
	};
}
template <typename T>
void from_json(const nlohmann::ordered_json& j, quaternion<T>& q) {
	q.x = j.value("x", T(0));
	q.y = j.value("y", T(0));
	q.z = j.value("z", T(0));
	q.w = j.value("w", T(1));
}
}
}
