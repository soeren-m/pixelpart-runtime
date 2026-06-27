#include "Types.h"

namespace pixelpart {
int_t* valuePtr(int_t& v) {
	return &v;
}
float_t* valuePtr(float_t& v) {
	return &v;
}
float_t* valuePtr(float2_t& v) {
	return &v.x;
}
float_t* valuePtr(float3_t& v) {
	return &v.x;
}
float_t* valuePtr(float4_t& v) {
	return &v.x;
}
float_t* valuePtr(matrix2_t& v) {
	return &v[0].x;
}
float_t* valuePtr(matrix3_t& v) {
	return &v[0].x;
}
float_t* valuePtr(matrix4_t& v) {
	return &v[0].x;
}

const int_t* valuePtr(const int_t& v) {
	return &v;
}
const float_t* valuePtr(const float_t& v) {
	return &v;
}
const float_t* valuePtr(const float2_t& v) {
	return &v.x;
}
const float_t* valuePtr(const float3_t& v) {
	return &v.x;
}
const float_t* valuePtr(const float4_t& v) {
	return &v.x;
}
const float_t* valuePtr(const matrix2_t& v) {
	return &v[0].x;
}
const float_t* valuePtr(const matrix3_t& v) {
	return &v[0].x;
}
const float_t* valuePtr(const matrix4_t& v) {
	return &v[0].x;
}
}
