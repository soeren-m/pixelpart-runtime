#pragma once

#include "../common/Types.h"

namespace pixelpart {
struct ShaderValue {
	enum Type : int32_t {
		type_null = -1,
		type_bool,
		type_int,
		type_float,
		type_float2,
		type_float3,
		type_float4
	} type = type_null;

	union {
		bool boolean;
		int32_t integer;
		floatd number[4];
	} data;

	static ShaderValue BoolValue(bool v);
	static ShaderValue IntValue(int32_t v);
	static ShaderValue FloatValue(floatd v);
	static ShaderValue Float2Value(const vec2d& v);
	static ShaderValue Float3Value(const vec3d& v);
	static ShaderValue Float4Value(const vec4d& v);

	ShaderValue();

	bool getBool() const;
	int32_t getInt() const;
	floatd getFloat() const;
	vec2d getFloat2() const;
	vec3d getFloat3() const;
	vec4d getFloat4() const;
};

NLOHMANN_JSON_SERIALIZE_ENUM(ShaderValue::Type, {
	{ ShaderValue::Type::type_null, nullptr },
	{ ShaderValue::Type::type_bool, "bool" },
	{ ShaderValue::Type::type_int, "int" },
	{ ShaderValue::Type::type_float, "float" },
	{ ShaderValue::Type::type_float2, "float2" },
	{ ShaderValue::Type::type_float3, "float3" },
	{ ShaderValue::Type::type_float4, "float4" }
})

void to_json(nlohmann::ordered_json& j, const ShaderValue& value);
void from_json(const nlohmann::ordered_json& j, ShaderValue& value);
}