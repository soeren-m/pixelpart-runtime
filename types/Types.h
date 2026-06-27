#pragma once

#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"
#include "../math/Matrix2x2.h"
#include "../math/Matrix3x3.h"
#include "../math/Matrix4x4.h"
#include <cstdint>
#include <cstddef>

namespace pixelpart {
using int_t = std::int64_t;
using float_t = double;
using float2_t = math::vector2<double>;
using float3_t = math::vector3<double>;
using float4_t = math::vector4<double>;
using matrix2_t = math::matrix2x2<double>;
using matrix3_t = math::matrix3x3<double>;
using matrix4_t = math::matrix4x4<double>;

int_t* valuePtr(int_t& v);
float_t* valuePtr(float_t& v);
float_t* valuePtr(float2_t& v);
float_t* valuePtr(float3_t& v);
float_t* valuePtr(float4_t& v);
float_t* valuePtr(matrix2_t& m);
float_t* valuePtr(matrix3_t& m);
float_t* valuePtr(matrix4_t& m);

const int_t* valuePtr(const int_t& v);
const float_t* valuePtr(const float_t& v);
const float_t* valuePtr(const float2_t& v);
const float_t* valuePtr(const float3_t& v);
const float_t* valuePtr(const float4_t& v);
const float_t* valuePtr(const matrix2_t& v);
const float_t* valuePtr(const matrix3_t& v);
const float_t* valuePtr(const matrix4_t& v);
}
