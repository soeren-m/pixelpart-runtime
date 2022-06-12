#pragma once

#include "Types.h"

namespace pixelpart {
enum class ShaderValueType : uint32_t {
	type_bool,
	type_int,
	type_float,
	type_float2,
	type_float3,
	type_float4
};

NLOHMANN_JSON_SERIALIZE_ENUM(ShaderValueType, {
	{ ShaderValueType::type_bool, "bool" },
	{ ShaderValueType::type_int, "int" },
	{ ShaderValueType::type_float, "float" },
	{ ShaderValueType::type_float2, "float2" },
	{ ShaderValueType::type_float3, "float3" },
	{ ShaderValueType::type_float4, "float4" }
})
}