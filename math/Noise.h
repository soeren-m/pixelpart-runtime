#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <cstdint>

namespace pixelpart {
namespace math {
namespace internal {
const std::uint8_t permutationTable[256] = {
	151, 160, 137, 91, 90, 15,
	131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
	190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
	88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
	77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
	102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
	135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
	5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
	223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
	129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
	251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
	49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
	138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
};

const std::int32_t simplexTable4d[64][4] = {
	{0, 1, 2, 3}, {0, 1, 3, 2}, {0, 0, 0, 0}, {0, 2, 3, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {1, 2, 3, 0},
	{0, 2, 1, 3}, {0, 0, 0, 0}, {0, 3, 1, 2}, {0, 3, 2, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {1, 3, 2, 0},
	{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
	{1, 2, 0, 3}, {0, 0, 0, 0}, {1, 3, 0, 2}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {2, 3, 0, 1}, {2, 3, 1, 0},
	{1, 0, 2, 3}, {1, 0, 3, 2}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {2, 0, 3, 1}, {0, 0, 0, 0}, {2, 1, 3, 0},
	{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
	{2, 0, 1, 3}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {3, 0, 1, 2}, {3, 0, 2, 1}, {0, 0, 0, 0}, {3, 1, 2, 0},
	{2, 1, 0, 3}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {3, 1, 0, 2}, {0, 0, 0, 0}, {3, 2, 0, 1}, {3, 2, 1, 0}
};

const std::int32_t gradTable4d[32][4] = {
	{+0, +1, +1, +1}, {+0, +1, +1, -1}, {+0, +1, -1, +1}, {+0, +1, -1, -1},
	{+0, -1, +1, +1}, {+0, -1, +1, -1}, {+0, -1, -1, +1}, {+0, -1, -1, -1},
	{+1, +0, +1, +1}, {+1, +0, +1, -1}, {+1, +0, -1, +1}, {+1, +0, -1, -1},
	{-1, +0, +1, +1}, {-1, +0, +1, -1}, {-1, +0, -1, +1}, {-1, +0, -1, -1},
	{+1, +1, +0, +1}, {+1, +1, +0, -1}, {+1, -1, +0, +1}, {+1, -1, +0, -1},
	{-1, +1, +0, +1}, {-1, +1, +0, -1}, {-1, -1, +0, +1}, {-1, -1, +0, -1},
	{+1, +1, +1, +0}, {+1, +1, -1, +0}, {+1, -1, +1, +0}, {+1, -1, -1, +0},
	{-1, +1, +1, +0}, {-1, +1, -1, +0}, {-1, -1, +1, +0}, {-1, -1, -1, +0}
};

inline std::uint8_t hash(std::int32_t i) {
	return permutationTable[static_cast<std::uint8_t>(i)];
}

template <typename T>
std::int32_t fastfloor(T f) {
	std::int32_t i = static_cast<std::int32_t>(f);
	return f < i ? i - 1 : i;
}

template <typename T>
T grad(std::int32_t hash, T x) {
	std::int32_t h = hash & 0x0F;
	T grad = T(1) + static_cast<T>(h & 7);

	return ((h & 8) ? -grad : grad) * x;
}
template <typename T>
T grad(std::int32_t hash, T x, T y) {
	std::int32_t h = hash & 0x3F;
	T u = h < 4 ? x : y;
	T v = h < 4 ? y : x;

	return ((h & 1) ? -u : u) + ((h & 2) ? T(-2) * v : T(2) * v);
}
template <typename T>
T grad(std::int32_t hash, T x, T y, T z) {
	std::int32_t h = hash & 15;
	T u = h < 8 ? x : y;
	T v = h < 4 ? y : h == 12 || h == 14 ? x : z;

	return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}
template <typename T>
T grad(std::int32_t hash, T x, T y, T z, T w) {
	return gradTable4d[hash][0] * x +
		gradTable4d[hash][1] * y +
		gradTable4d[hash][2] * z +
		gradTable4d[hash][3] * w;
}
}

template <typename T>
T simplexNoise(T p) {
	std::int32_t i0 = internal::fastfloor(p);
	std::int32_t i1 = i0 + 1;

	T x0 = p - i0;
	T t0 = T(1) - x0 * x0;
	t0 *= t0;
	T n0 = t0 * t0 * internal::grad(internal::hash(i0), x0);

	T x1 = x0 - T(1);
	T t1 = T(1) - x1 * x1;
	t1 *= t1;
	T n1 = t1 * t1 * internal::grad(internal::hash(i1), x1);

	return T(0.395) * (n0 + n1);
}
template <typename T>
T simplexNoise(const vector2<T>& p) {
	constexpr T F2 = T(0.366025403);
	constexpr T G2 = T(0.211324865);

	const T s = (p.x + p.y) * F2;
	const T xs = p.x + s;
	const T ys = p.y + s;
	const std::int32_t i = internal::fastfloor(xs);
	const std::int32_t j = internal::fastfloor(ys);

	const T t = static_cast<T>(i + j) * G2;
	const T X0 = i - t;
	const T Y0 = j - t;
	const T x0 = p.x - X0;
	const T y0 = p.y - Y0;

	std::int32_t i1, j1;
	if (x0 > y0) {
		i1 = 1;
		j1 = 0;
	}
	else {
		i1 = 0;
		j1 = 1;
	}

	const T x1 = x0 - i1 + G2;
	const T y1 = y0 - j1 + G2;
	const T x2 = x0 - T(1) + T(2) * G2;
	const T y2 = y0 - T(1) + T(2) * G2;

	const std::int32_t gi0 = internal::hash(i + internal::hash(j));
	const std::int32_t gi1 = internal::hash(i + i1 + internal::hash(j + j1));
	const std::int32_t gi2 = internal::hash(i + 1 + internal::hash(j + 1));

	T n0;
	T t0 = T(0.5) - x0 * x0 - y0 * y0;
	if(t0 < T(0)) {
		n0 = T(0);
	}
	else {
		t0 *= t0;
		n0 = t0 * t0 * internal::grad(gi0, x0, y0);
	}

	T n1;
	T t1 = T(0.5) - x1*x1 - y1*y1;
	if (t1 < T(0)) {
		n1 = T(0);
	}
	else {
		t1 *= t1;
		n1 = t1 * t1 * internal::grad(gi1, x1, y1);
	}

	T n2;
	T t2 = T(0.5) - x2*x2 - y2*y2;
	if (t2 < T(0)) {
		n2 = T(0);
	}
	else {
		t2 *= t2;
		n2 = t2 * t2 * internal::grad(gi2, x2, y2);
	}

	return T(45.23065) * (n0 + n1 + n2);
}
template <typename T>
T simplexNoise(const vector3<T>& p) {
	constexpr T F3 = T(1.0 / 3.0);
	constexpr T G3 = T(1.0 / 6.0);

	T s = (p.x + p.y + p.z) * F3;
	std::int32_t i = internal::fastfloor(p.x + s);
	std::int32_t j = internal::fastfloor(p.y + s);
	std::int32_t k = internal::fastfloor(p.z + s);
	T t = (i + j + k) * G3;
	T X0 = i - t;
	T Y0 = j - t;
	T Z0 = k - t;
	T x0 = p.x - X0;
	T y0 = p.y - Y0;
	T z0 = p.z - Z0;

	std::int32_t i1, j1, k1;
	std::int32_t i2, j2, k2;
	if (x0 >= y0) {
		if (y0 >= z0) {
			i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0;
		}
		else if (x0 >= z0) {
			i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1;
		}
		else {
			i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1;
		}
	}
	else {
		if (y0 < z0) {
			i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1;
		}
		else if (x0 < z0) {
			i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1;
		}
		else {
			i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0;
		}
	}

	T x1 = x0 - i1 + G3;
	T y1 = y0 - j1 + G3;
	T z1 = z0 - k1 + G3;
	T x2 = x0 - i2 + T(2) * G3;
	T y2 = y0 - j2 + T(2) * G3;
	T z2 = z0 - k2 + T(2) * G3;
	T x3 = x0 - T(1) + T(3) * G3;
	T y3 = y0 - T(1) + T(3) * G3;
	T z3 = z0 - T(1) + T(3) * G3;

	std::int32_t gi0 = internal::hash(i + internal::hash(j + internal::hash(k)));
	std::int32_t gi1 = internal::hash(i + i1 + internal::hash(j + j1 + internal::hash(k + k1)));
	std::int32_t gi2 = internal::hash(i + i2 + internal::hash(j + j2 + internal::hash(k + k2)));
	std::int32_t gi3 = internal::hash(i + 1 + internal::hash(j + 1 + internal::hash(k + 1)));

	T n0;
	T t0 = T(0.6) - x0 * x0 - y0 * y0 - z0 * z0;
	if (t0 < 0) {
		n0 = T(0);
	}
	else {
		t0 *= t0;
		n0 = t0 * t0 * internal::grad(gi0, x0, y0, z0);
	}

	T n1;
	T t1 = T(0.6) - x1 * x1 - y1 * y1 - z1 * z1;
	if (t1 < 0) {
		n1 = T(0);
	}
	else {
		t1 *= t1;
		n1 = t1 * t1 * internal::grad(gi1, x1, y1, z1);
	}

	T n2;
	T t2 = T(0.6) - x2 * x2 - y2 * y2 - z2 * z2;
	if (t2 < 0) {
		n2 = T(0);
	}
	else {
		t2 *= t2;
		n2 = t2 * t2 * internal::grad(gi2, x2, y2, z2);
	}

	T n3;
	T t3 = T(0.6) - x3 * x3 - y3 * y3 - z3 * z3;
	if (t3 < 0) {
		n3 = T(0);
	}
	else {
		t3 *= t3;
		n3 = t3 * t3 * internal::grad(gi3, x3, y3, z3);
	}

	return T(32) * (n0 + n1 + n2 + n3);
}
template <typename T>
T simplexNoise(const vector4<T>& p) {
	constexpr T F4 = T(0.3090169944);
	constexpr T G4 = T(0.1381966011);

	T s = (p.x + p.y + p.z + p.w) * F4;
	std::int32_t i = internal::fastfloor(p.x + s);
	std::int32_t j = internal::fastfloor(p.y + s);
	std::int32_t k = internal::fastfloor(p.z + s);
	std::int32_t l = internal::fastfloor(p.w + s);
	T t = (i + j + k + l) * G4;

	T X0 = i - t;
	T Y0 = j - t;
	T Z0 = k - t;
	T W0 = l - t;
	T x0 = p.x - X0;
	T y0 = p.y - Y0;
	T z0 = p.z - Z0;
	T w0 = p.w - W0;

	std::int32_t c1 = (x0 > y0) ? 32 : 0;
	std::int32_t c2 = (x0 > z0) ? 16 : 0;
	std::int32_t c3 = (y0 > z0) ? 8 : 0;
	std::int32_t c4 = (x0 > w0) ? 4 : 0;
	std::int32_t c5 = (y0 > w0) ? 2 : 0;
	std::int32_t c6 = (z0 > w0) ? 1 : 0;
	std::int32_t c = c1 + c2 + c3 + c4 + c5 + c6;

	std::int32_t i1 = internal::simplexTable4d[c][0] >= 3 ? 1 : 0;
	std::int32_t j1 = internal::simplexTable4d[c][1] >= 3 ? 1 : 0;
	std::int32_t k1 = internal::simplexTable4d[c][2] >= 3 ? 1 : 0;
	std::int32_t l1 = internal::simplexTable4d[c][3] >= 3 ? 1 : 0;

	std::int32_t i2 = internal::simplexTable4d[c][0] >= 2 ? 1 : 0;
	std::int32_t j2 = internal::simplexTable4d[c][1] >= 2 ? 1 : 0;
	std::int32_t k2 = internal::simplexTable4d[c][2] >= 2 ? 1 : 0;
	std::int32_t l2 = internal::simplexTable4d[c][3] >= 2 ? 1 : 0;

	std::int32_t i3 = internal::simplexTable4d[c][0] >= 1 ? 1 : 0;
	std::int32_t j3 = internal::simplexTable4d[c][1] >= 1 ? 1 : 0;
	std::int32_t k3 = internal::simplexTable4d[c][2] >= 1 ? 1 : 0;
	std::int32_t l3 = internal::simplexTable4d[c][3] >= 1 ? 1 : 0;

	T x1 = x0 - i1 + G4;
	T y1 = y0 - j1 + G4;
	T z1 = z0 - k1 + G4;
	T w1 = w0 - l1 + G4;
	T x2 = x0 - i2 + T(2) * G4;
	T y2 = y0 - j2 + T(2) * G4;
	T z2 = z0 - k2 + T(2) * G4;
	T w2 = w0 - l2 + T(2) * G4;
	T x3 = x0 - i3 + T(3) * G4;
	T y3 = y0 - j3 + T(3) * G4;
	T z3 = z0 - k3 + T(3) * G4;
	T w3 = w0 - l3 + T(3) * G4;
	T x4 = x0 - T(1) + T(4) * G4;
	T y4 = y0 - T(1) + T(4) * G4;
	T z4 = z0 - T(1) + T(4) * G4;
	T w4 = w0 - T(1) + T(4) * G4;

	std::int32_t ii = i & 255;
	std::int32_t jj = j & 255;
	std::int32_t kk = k & 255;
	std::int32_t ll = l & 255;
	std::int32_t gi0 = internal::hash(ii + internal::hash(jj + internal::hash(kk + internal::hash(ll)))) % 32;
	std::int32_t gi1 = internal::hash(ii + i1 + internal::hash(jj + j1 + internal::hash(kk + k1 + internal::hash(ll + l1)))) % 32;
	std::int32_t gi2 = internal::hash(ii + i2 + internal::hash(jj + j2 + internal::hash(kk + k2 + internal::hash(ll + l2)))) % 32;
	std::int32_t gi3 = internal::hash(ii + i3 + internal::hash(jj + j3 + internal::hash(kk + k3 + internal::hash(ll + l3)))) % 32;
	std::int32_t gi4 = internal::hash(ii + 1 + internal::hash(jj + 1 + internal::hash(kk + 1 + internal::hash(ll + 1)))) % 32;

	T n0;
	T t0 = T(0.6) - x0 * x0 - y0 * y0 - z0 * z0 - w0 * w0;
	if(t0 < T(0)) {
		n0 = T(0);
	}
	else {
		t0 *= t0;
		n0 = t0 * t0 * internal::grad(gi0, x0, y0, z0, w0);
	}

	T n1;
	T t1 = T(0.6) - x1 * x1 - y1 * y1 - z1 * z1 - w1 * w1;
	if(t1 < T(0)) {
		n1 = T(0);
	}
	else {
		t1 *= t1;
		n1 = t1 * t1 * internal::grad(gi1, x1, y1, z1, w1);
	}

	T n2;
	T t2 = T(0.6) - x2 * x2 - y2 * y2 - z2 * z2 - w2 * w2;
	if(t2 < T(0)) {
		n2 = T(0);
	}
	else {
		t2 *= t2;
		n2 = t2 * t2 * internal::grad(gi2, x2, y2, z2, w2);
	}

	T n3;
	T t3 = T(0.6) - x3 * x3 - y3 * y3 - z3 * z3 - w3 * w3;
	if(t3 < T(0)) {
		n3 = T(0);
	}
	else {
		t3 *= t3;
		n3 = t3 * t3 * internal::grad(gi3, x3, y3, z3, w3);
	}

	T n4;
	T t4 = T(0.6) - x4 * x4 - y4 * y4 - z4 * z4 - w4 * w4;
	if(t4 < T(0)) {
		n4 = T(0);
	}
	else {
		t4 *= t4;
		n4 = t4 * t4 * internal::grad(gi4, x4, y4, z4, w4);
	}

	return T(27) * (n0 + n1 + n2 + n3 + n4);
}

template <typename T, typename TVec>
T simplexNoiseFBM(std::uint32_t octaves, T frequency, T persistence, T lacunarity, const TVec& position) {
	if(octaves == 0) {
		return T(0);
	}

	T result = T(0);
	T amplitude = T(1);
	T denom = T(0);

	for(std::uint32_t octave = 0; octave < octaves; octave++) {
		result += amplitude * simplexNoise(position * frequency);
		denom += amplitude;

		amplitude *= persistence;
		frequency *= lacunarity;
	}

	return result / denom;

}
}
}
