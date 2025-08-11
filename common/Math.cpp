#include "Math.h"

namespace pixelpart {
const float2_t worldUpVector2 = float2_t(0.0, 1.0);
const float3_t worldUpVector3 = float3_t(0.0, 1.0, 0.0);
const float4_t worldUpVector4 = float4_t(0.0, 1.0, 0.0, 0.0);

pixelpart::float_t* valuePtr(pixelpart::float2_t& v) {
	return &(v.x);
}
pixelpart::float_t* valuePtr(pixelpart::float3_t& v) {
	return &(v.x);
}
pixelpart::float_t* valuePtr(pixelpart::float4_t& v) {
	return &(v.x);
}
pixelpart::float_t* valuePtr(pixelpart::mat2_t& v) {
	return &(v[0].x);
}
pixelpart::float_t* valuePtr(pixelpart::mat3_t& v) {
	return &(v[0].x);
}
pixelpart::float_t* valuePtr(pixelpart::mat4_t& v) {
	return &(v[0].x);
}

const pixelpart::float_t* valuePtr(const pixelpart::float2_t& v) {
	return &(v.x);
}
const pixelpart::float_t* valuePtr(const pixelpart::float3_t& v) {
	return &(v.x);
}
const pixelpart::float_t* valuePtr(const pixelpart::float4_t& v) {
	return &(v.x);
}
const pixelpart::float_t* valuePtr(const pixelpart::mat2_t& v) {
	return &(v[0].x);
}
const pixelpart::float_t* valuePtr(const pixelpart::mat3_t& v) {
	return &(v[0].x);
}
const pixelpart::float_t* valuePtr(const pixelpart::mat4_t& v) {
	return &(v[0].x);
}

void to_json(nlohmann::ordered_json& j, const float2_t& v) {
	j = nlohmann::ordered_json{
		{ "x", v.x },
		{ "y", v.y },
	};
}
void to_json(nlohmann::ordered_json& j, const float3_t& v) {
	j = nlohmann::ordered_json{
		{ "x", v.x },
		{ "y", v.y },
		{ "z", v.z },
	};
}
void to_json(nlohmann::ordered_json& j, const float4_t& v) {
	j = nlohmann::ordered_json{
		{ "x", v.x },
		{ "y", v.y },
		{ "z", v.z },
		{ "w", v.w },
	};
}
void from_json(const nlohmann::ordered_json& j, float2_t& v) {
	if(j.is_number()) {
		v.x = j.get<float_t>();
		v.y = 0.0;
	}
	else {
		v.x = j.contains("x") ? j.at("x").get<float_t>() : 0.0;
		v.y = j.contains("y") ? j.at("y").get<float_t>() : 0.0;
	}
}
void from_json(const nlohmann::ordered_json& j, float3_t& v) {
	if(j.is_number()) {
		v.x = j.get<float_t>();
		v.y = 0.0;
		v.z = 0.0;
	}
	else {
		v.x = j.contains("x") ? j.at("x").get<float_t>() : 0.0;
		v.y = j.contains("y") ? j.at("y").get<float_t>() : 0.0;
		v.z = j.contains("z") ? j.at("z").get<float_t>() : 0.0;
	}
}
void from_json(const nlohmann::ordered_json& j, float4_t& v) {
	if(j.is_number()) {
		v.x = j.get<float_t>();
		v.y = 0.0;
		v.z = 0.0;
		v.w = 0.0;
	}
	else {
		v.x = j.contains("x") ? j.at("x").get<float_t>() : 0.0;
		v.y = j.contains("y") ? j.at("y").get<float_t>() : 0.0;
		v.z = j.contains("z") ? j.at("z").get<float_t>() : 0.0;
		v.w = j.contains("w") ? j.at("w").get<float_t>() : 0.0;
	}
}
}
