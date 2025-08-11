#pragma once

#include "Types.h"
#include "../glm/glm.hpp"
#include "../json/json.hpp"

namespace pixelpart {
using float2_t = glm::vec<2, float_t, glm::highp>;
using float3_t = glm::vec<3, float_t, glm::highp>;
using float4_t = glm::vec<4, float_t, glm::highp>;
using mat2_t = glm::mat<2, 2, float_t, glm::highp>;
using mat3_t = glm::mat<3, 3, float_t, glm::highp>;
using mat4_t = glm::mat<4, 4, float_t, glm::highp>;

extern const float2_t worldUpVector2;
extern const float3_t worldUpVector3;
extern const float4_t worldUpVector4;

pixelpart::float_t* valuePtr(pixelpart::float2_t& v);
pixelpart::float_t* valuePtr(pixelpart::float3_t& v);
pixelpart::float_t* valuePtr(pixelpart::float4_t& v);
pixelpart::float_t* valuePtr(pixelpart::mat2_t& v);
pixelpart::float_t* valuePtr(pixelpart::mat3_t& v);
pixelpart::float_t* valuePtr(pixelpart::mat4_t& v);

const pixelpart::float_t* valuePtr(const pixelpart::float2_t& v);
const pixelpart::float_t* valuePtr(const pixelpart::float3_t& v);
const pixelpart::float_t* valuePtr(const pixelpart::float4_t& v);
const pixelpart::float_t* valuePtr(const pixelpart::mat2_t& v);
const pixelpart::float_t* valuePtr(const pixelpart::mat3_t& v);
const pixelpart::float_t* valuePtr(const pixelpart::mat4_t& v);

void to_json(nlohmann::ordered_json& j, const float2_t& v);
void to_json(nlohmann::ordered_json& j, const float3_t& v);
void to_json(nlohmann::ordered_json& j, const float4_t& v);
void from_json(const nlohmann::ordered_json& j, float2_t& v);
void from_json(const nlohmann::ordered_json& j, float3_t& v);
void from_json(const nlohmann::ordered_json& j, float4_t& v);
}

namespace nlohmann {
template <>
struct adl_serializer<pixelpart::float2_t> {
	static void to_json(ordered_json& j, const pixelpart::float2_t& v) {
		pixelpart::to_json(j, v);
	}
	static void from_json(const ordered_json& j, pixelpart::float2_t& v) {
		pixelpart::from_json(j, v);
	}
};

template <>
struct adl_serializer<pixelpart::float3_t> {
	static void to_json(ordered_json& j, const pixelpart::float3_t& v) {
		pixelpart::to_json(j, v);
	}
	static void from_json(const ordered_json& j, pixelpart::float3_t& v) {
		pixelpart::from_json(j, v);
	}
};

template <>
struct adl_serializer<pixelpart::float4_t> {
	static void to_json(ordered_json& j, const pixelpart::float4_t& v) {
		pixelpart::to_json(j, v);
	}
	static void from_json(const ordered_json& j, pixelpart::float4_t& v) {
		pixelpart::from_json(j, v);
	}
};
}
