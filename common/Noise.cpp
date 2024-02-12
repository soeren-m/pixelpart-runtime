#include "Noise.h"

namespace pixelpart {
namespace noise {
const uint8_t permutationTable[256] = {
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

const int32_t simplexTable4d[64][4] = {
	{0, 1, 2, 3}, {0, 1, 3, 2}, {0, 0, 0, 0}, {0, 2, 3, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {1, 2, 3, 0},
	{0, 2, 1, 3}, {0, 0, 0, 0}, {0, 3, 1, 2}, {0, 3, 2, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {1, 3, 2, 0},
	{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
	{1, 2, 0, 3}, {0, 0, 0, 0}, {1, 3, 0, 2}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {2, 3, 0, 1}, {2, 3, 1, 0},
	{1, 0, 2, 3}, {1, 0, 3, 2}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {2, 0, 3, 1}, {0, 0, 0, 0}, {2, 1, 3, 0},
	{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
	{2, 0, 1, 3}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {3, 0, 1, 2}, {3, 0, 2, 1}, {0, 0, 0, 0}, {3, 1, 2, 0},
	{2, 1, 0, 3}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {3, 1, 0, 2}, {0, 0, 0, 0}, {3, 2, 0, 1}, {3, 2, 1, 0}
};

const int32_t gradTable4d[32][4] = {
	{+0, +1, +1, +1}, {+0, +1, +1, -1}, {+0, +1, -1, +1}, {+0, +1, -1, -1},
	{+0, -1, +1, +1}, {+0, -1, +1, -1}, {+0, -1, -1, +1}, {+0, -1, -1, -1},
	{+1, +0, +1, +1}, {+1, +0, +1, -1}, {+1, +0, -1, +1}, {+1, +0, -1, -1},
	{-1, +0, +1, +1}, {-1, +0, +1, -1}, {-1, +0, -1, +1}, {-1, +0, -1, -1},
	{+1, +1, +0, +1}, {+1, +1, +0, -1}, {+1, -1, +0, +1}, {+1, -1, +0, -1},
	{-1, +1, +0, +1}, {-1, +1, +0, -1}, {-1, -1, +0, +1}, {-1, -1, +0, -1},
	{+1, +1, +1, +0}, {+1, +1, -1, +0}, {+1, -1, +1, +0}, {+1, -1, -1, +0},
	{-1, +1, +1, +0}, {-1, +1, -1, +0}, {-1, -1, +1, +0}, {-1, -1, -1, +0}
};

inline uint8_t hash(int32_t i) {
    return permutationTable[static_cast<uint8_t>(i)];
}

inline int32_t fastfloor(float_t f) {
    int32_t i = static_cast<int32_t>(f);
    return f < i ? i - 1 : i;
}

float_t grad(int32_t hash, float_t x) {
	int32_t h = hash & 0x0F;
	float_t grad = 1.0 + (h & 7);

	return ((h & 8) ? -grad : grad) * x;
}
float_t grad(int32_t hash, float_t x, float_t y) {
	int32_t h = hash & 0x3F;
	float_t u = h < 4 ? x : y;
	float_t v = h < 4 ? y : x;

	return ((h & 1) ? -u : u) + ((h & 2) ? -2.0 * v : 2.0 * v);
}
float_t grad(int32_t hash, float_t x, float_t y, float_t z) {
	int32_t h = hash & 15;
	float_t u = h < 8 ? x : y;
	float_t v = h < 4 ? y : h == 12 || h == 14 ? x : z;

	return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}
float_t grad(int32_t hash, float_t x, float_t y, float_t z, float_t w) {
	return gradTable4d[hash][0] * x +
		gradTable4d[hash][1] * y +
		gradTable4d[hash][2] * z +
		gradTable4d[hash][3] * w;
}

float_t simplexNoise(float_t x) {
	int32_t i0 = fastfloor(x);
	int32_t i1 = i0 + 1;

	float_t x0 = x - i0;
	float_t t0 = 1.0 - x0 * x0;
	t0 *= t0;
	float_t n0 = t0 * t0 * grad(hash(i0), x0);

	float_t x1 = x0 - 1.0;
	float_t t1 = 1.0 - x1 * x1;
	t1 *= t1;
	float_t n1 = t1 * t1 * grad(hash(i1), x1);

	return 0.395f * (n0 + n1);
}
float_t simplexNoise(float_t x, float_t y) {
	static const float_t F2 = 0.366025403;
	static const float_t G2 = 0.211324865;

	const float_t s = (x + y) * F2;
	const float_t xs = x + s;
	const float_t ys = y + s;
	const int32_t i = fastfloor(xs);
	const int32_t j = fastfloor(ys);

	const float_t t = static_cast<float_t>(i + j) * G2;
	const float_t X0 = i - t;
	const float_t Y0 = j - t;
	const float_t x0 = x - X0;
	const float_t y0 = y - Y0;

	int32_t i1, j1;
	if (x0 > y0) {
		i1 = 1;
		j1 = 0;
	}
	else {
		i1 = 0;
		j1 = 1;
	}

	const float_t x1 = x0 - i1 + G2;
	const float_t y1 = y0 - j1 + G2;
	const float_t x2 = x0 - 1.0 + 2.0 * G2;
	const float_t y2 = y0 - 1.0 + 2.0 * G2;

	const int32_t gi0 = hash(i + hash(j));
	const int32_t gi1 = hash(i + i1 + hash(j + j1));
	const int32_t gi2 = hash(i + 1 + hash(j + 1));

	float_t n0;
	float_t t0 = 0.5 - x0 * x0 - y0 * y0;
	if(t0 < 0.0) {
		n0 = 0.0;
	}
	else {
		t0 *= t0;
		n0 = t0 * t0 * grad(gi0, x0, y0);
	}

	float_t n1;
	float_t t1 = 0.5 - x1*x1 - y1*y1;
	if (t1 < 0.0) {
		n1 = 0.0;
	}
	else {
		t1 *= t1;
		n1 = t1 * t1 * grad(gi1, x1, y1);
	}

	float_t n2;
	float_t t2 = 0.5 - x2*x2 - y2*y2;
	if (t2 < 0.0) {
		n2 = 0.0;
	}
	else {
		t2 *= t2;
		n2 = t2 * t2 * grad(gi2, x2, y2);
	}

	return 45.23065 * (n0 + n1 + n2);
}
float_t simplexNoise(float_t x, float_t y, float_t z) {
	static const float_t F3 = 1.0 / 3.0;
	static const float_t G3 = 1.0 / 6.0;

	float_t s = (x + y + z) * F3;
	int32_t i = fastfloor(x + s);
	int32_t j = fastfloor(y + s);
	int32_t k = fastfloor(z + s);
	float_t t = (i + j + k) * G3;
	float_t X0 = i - t;
	float_t Y0 = j - t;
	float_t Z0 = k - t;
	float_t x0 = x - X0;
	float_t y0 = y - Y0;
	float_t z0 = z - Z0;

	int32_t i1, j1, k1;
	int32_t i2, j2, k2;
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

	float_t x1 = x0 - i1 + G3;
	float_t y1 = y0 - j1 + G3;
	float_t z1 = z0 - k1 + G3;
	float_t x2 = x0 - i2 + 2.0 * G3;
	float_t y2 = y0 - j2 + 2.0 * G3;
	float_t z2 = z0 - k2 + 2.0 * G3;
	float_t x3 = x0 - 1.0 + 3.0 * G3;
	float_t y3 = y0 - 1.0 + 3.0 * G3;
	float_t z3 = z0 - 1.0 + 3.0 * G3;

	int32_t gi0 = hash(i + hash(j + hash(k)));
	int32_t gi1 = hash(i + i1 + hash(j + j1 + hash(k + k1)));
	int32_t gi2 = hash(i + i2 + hash(j + j2 + hash(k + k2)));
	int32_t gi3 = hash(i + 1 + hash(j + 1 + hash(k + 1)));

	float_t n0;
	float_t t0 = 0.6 - x0 * x0 - y0 * y0 - z0 * z0;
	if (t0 < 0) {
		n0 = 0.0;
	}
	else {
		t0 *= t0;
		n0 = t0 * t0 * grad(gi0, x0, y0, z0);
	}

	float_t n1;
	float_t t1 = 0.6 - x1 * x1 - y1 * y1 - z1 * z1;
	if (t1 < 0) {
		n1 = 0.0;
	}
	else {
		t1 *= t1;
		n1 = t1 * t1 * grad(gi1, x1, y1, z1);
	}

	float_t n2;
	float_t t2 = 0.6 - x2 * x2 - y2 * y2 - z2 * z2;
	if (t2 < 0) {
		n2 = 0.0;
	}
	else {
		t2 *= t2;
		n2 = t2 * t2 * grad(gi2, x2, y2, z2);
	}

	float_t n3;
	float_t t3 = 0.6 - x3 * x3 - y3 * y3 - z3 * z3;
	if (t3 < 0) {
		n3 = 0.0;
	}
	else {
		t3 *= t3;
		n3 = t3 * t3 * grad(gi3, x3, y3, z3);
	}

	return 32.0 * (n0 + n1 + n2 + n3);
}
float_t simplexNoise(float_t x, float_t y, float_t z, float_t w) {
	static const float_t F4 = 0.3090169944;
	static const float_t G4 = 0.1381966011;

	float_t s = (x + y + z + w) * F4;
	int32_t i = fastfloor(x + s);
	int32_t j = fastfloor(y + s);
	int32_t k = fastfloor(z + s);
	int32_t l = fastfloor(w + s);
	float_t t = (i + j + k + l) * G4;

	float_t X0 = i - t;
	float_t Y0 = j - t;
	float_t Z0 = k - t;
	float_t W0 = l - t;
	float_t x0 = x - X0;
	float_t y0 = y - Y0;
	float_t z0 = z - Z0;
	float_t w0 = w - W0;

	int32_t c1 = (x0 > y0) ? 32 : 0;
	int32_t c2 = (x0 > z0) ? 16 : 0;
	int32_t c3 = (y0 > z0) ? 8 : 0;
	int32_t c4 = (x0 > w0) ? 4 : 0;
	int32_t c5 = (y0 > w0) ? 2 : 0;
	int32_t c6 = (z0 > w0) ? 1 : 0;
	int32_t c = c1 + c2 + c3 + c4 + c5 + c6;

	int32_t i1 = simplexTable4d[c][0] >= 3 ? 1 : 0;
	int32_t j1 = simplexTable4d[c][1] >= 3 ? 1 : 0;
	int32_t k1 = simplexTable4d[c][2] >= 3 ? 1 : 0;
	int32_t l1 = simplexTable4d[c][3] >= 3 ? 1 : 0;

	int32_t i2 = simplexTable4d[c][0] >= 2 ? 1 : 0;
	int32_t j2 = simplexTable4d[c][1] >= 2 ? 1 : 0;
	int32_t k2 = simplexTable4d[c][2] >= 2 ? 1 : 0;
	int32_t l2 = simplexTable4d[c][3] >= 2 ? 1 : 0;

	int32_t i3 = simplexTable4d[c][0] >= 1 ? 1 : 0;
	int32_t j3 = simplexTable4d[c][1] >= 1 ? 1 : 0;
	int32_t k3 = simplexTable4d[c][2] >= 1 ? 1 : 0;
	int32_t l3 = simplexTable4d[c][3] >= 1 ? 1 : 0;

	float_t x1 = x0 - i1 + G4;
	float_t y1 = y0 - j1 + G4;
	float_t z1 = z0 - k1 + G4;
	float_t w1 = w0 - l1 + G4;
	float_t x2 = x0 - i2 + 2.0 * G4;
	float_t y2 = y0 - j2 + 2.0 * G4;
	float_t z2 = z0 - k2 + 2.0 * G4;
	float_t w2 = w0 - l2 + 2.0 * G4;
	float_t x3 = x0 - i3 + 3.0 * G4;
	float_t y3 = y0 - j3 + 3.0 * G4;
	float_t z3 = z0 - k3 + 3.0 * G4;
	float_t w3 = w0 - l3 + 3.0 * G4;
	float_t x4 = x0 - 1.0 + 4.0 * G4;
	float_t y4 = y0 - 1.0 + 4.0 * G4;
	float_t z4 = z0 - 1.0 + 4.0 * G4;
	float_t w4 = w0 - 1.0 + 4.0 * G4;

	int32_t ii = i & 255;
	int32_t jj = j & 255;
	int32_t kk = k & 255;
	int32_t ll = l & 255;
	int32_t gi0 = hash(ii + hash(jj + hash(kk + hash(ll)))) % 32;
	int32_t gi1 = hash(ii + i1 + hash(jj + j1 + hash(kk + k1 + hash(ll + l1)))) % 32;
	int32_t gi2 = hash(ii + i2 + hash(jj + j2 + hash(kk + k2 + hash(ll + l2)))) % 32;
	int32_t gi3 = hash(ii + i3 + hash(jj + j3 + hash(kk + k3 + hash(ll + l3)))) % 32;
	int32_t gi4 = hash(ii + 1 + hash(jj + 1 + hash(kk + 1 + hash(ll + 1)))) % 32;

	float_t n0;
	float_t t0 = 0.6 - x0 * x0 - y0 * y0 - z0 * z0 - w0 * w0;
	if(t0 < 0.0) {
		n0 = 0.0;
	}
	else {
		t0 *= t0;
		n0 = t0 * t0 * grad(gi0, x0, y0, z0, w0);
	}

	float_t n1;
	float_t t1 = 0.6 - x1 * x1 - y1 * y1 - z1 * z1 - w1 * w1;
	if(t1 < 0.0) {
		n1 = 0.0;
	}
	else {
		t1 *= t1;
		n1 = t1 * t1 * grad(gi1, x1, y1, z1, w1);
	}

	float_t n2;
	float_t t2 = 0.6 - x2 * x2 - y2 * y2 - z2 * z2 - w2 * w2;
	if(t2 < 0.0) {
		n2 = 0.0;
	}
	else {
		t2 *= t2;
		n2 = t2 * t2 * grad(gi2, x2, y2, z2, w2);
	}

	float_t n3;
	float_t t3 = 0.6 - x3 * x3 - y3 * y3 - z3 * z3 - w3 * w3;
	if(t3 < 0.0) {
		n3 = 0.0;
	}
	else {
		t3 *= t3;
		n3 = t3 * t3 * grad(gi3, x3, y3, z3, w3);
	}

	float_t n4;
	float_t t4 = 0.6 - x4 * x4 - y4 * y4 - z4 * z4 - w4 * w4;
	if(t4 < 0.0) {
		n4 = 0.0;
	}
	else {
		t4 *= t4;
		n4 = t4 * t4 * grad(gi4, x4, y4, z4, w4);
	}

	return 27.0 * (n0 + n1 + n2 + n3 + n4);
}

float_t fBmSimplexNoise(uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity, float_t x) {
	float_t result = 0.0;
	float_t amplitude = 1.0;
	float_t denom = 0.0;

	for(uint32_t o = 0u; o < octaves; o++) {
		result += amplitude * simplexNoise(x * frequency);
		denom += amplitude;

		amplitude *= persistence;
		frequency *= lacunarity;
	}

	return result / denom;
}
float_t fBmSimplexNoise(uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity, float_t x, float_t y) {
	float_t result = 0.0;
	float_t amplitude = 1.0;
	float_t denom = 0.0;

	for(uint32_t o = 0u; o < octaves; o++) {
		result += amplitude * simplexNoise(x * frequency, y * frequency);
		denom += amplitude;

		amplitude *= persistence;
		frequency *= lacunarity;
	}

	return result / denom;
}
float_t fBmSimplexNoise(uint32_t octaves, float_t frequency, float_t persistence, float_t lacunarity, float_t x, float_t y, float_t z) {
	float_t result = 0.0;
	float_t amplitude = 1.0;
	float_t denom = 0.0;

	for(uint32_t o = 0u; o < octaves; o++) {
		result += amplitude * simplexNoise(x * frequency, y * frequency, z * frequency);
		denom += amplitude;

		amplitude *= persistence;
		frequency *= lacunarity;
	}

	return result / denom;
}
float_t fBmSimplexNoise(uint32_t octaves,  float_t frequency, float_t persistence, float_t lacunarity, float_t x, float_t y, float_t z, float_t w) {
	float_t result = 0.0;
	float_t amplitude = 1.0;
	float_t denom = 0.0;

	for(uint32_t o = 0u; o < octaves; o++) {
		result += amplitude * simplexNoise(x * frequency, y * frequency, z * frequency, w * frequency);
		denom += amplitude;

		amplitude *= persistence;
		frequency *= lacunarity;
	}

	return result / denom;
}
}
}