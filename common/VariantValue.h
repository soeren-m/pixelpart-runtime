#pragma once

#include "../common/Types.h"

namespace pixelpart {
struct BoolTag { };
struct IntTag { };
struct FloatTag { };
struct Float2Tag { };
struct Float3Tag { };
struct Float4Tag { };

template <typename T>
struct VariantTag;
template <>
struct VariantTag<bool> { typedef BoolTag Type; };
template <>
struct VariantTag<int64_t> { typedef IntTag Type; };
template <>
struct VariantTag<floatd> { typedef FloatTag Type; };
template <>
struct VariantTag<vec2d> { typedef Float2Tag Type; };
template <>
struct VariantTag<vec3d> { typedef Float3Tag Type; };
template <>
struct VariantTag<vec4d> { typedef Float4Tag Type; };

struct VariantValue {
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
		int64_t integer;
		floatd number[4];
	} data;

	static VariantValue Bool(bool v);
	static VariantValue Int(int64_t v);
	static VariantValue Float(floatd v);
	static VariantValue Float2(const vec2d& v);
	static VariantValue Float3(const vec3d& v);
	static VariantValue Float4(const vec4d& v);

	VariantValue();
	VariantValue(Type t);

	template <typename T>
	T get() const {
		typedef typename VariantTag<T>::Type TagType;

		return get(TagType());
	}

	bool get(BoolTag t) const;
	int64_t get(IntTag t) const;
	floatd get(FloatTag t) const;
	vec2d get(Float2Tag t) const;
	vec3d get(Float3Tag t) const;
	vec4d get(Float4Tag t) const;

	bool getBool() const;
	int64_t getInt() const;
	floatd getFloat() const;
	vec2d getFloat2() const;
	vec3d getFloat3() const;
	vec4d getFloat4() const;

	bool toBool() const;
	int64_t toInt() const;
	floatd toFloat() const;
	vec2d toFloat2() const;
	vec3d toFloat3() const;
	vec4d toFloat4() const;

	VariantValue cast(Type type) const;
};

VariantValue operator+(const VariantValue& v1, const VariantValue& v2);
VariantValue operator-(const VariantValue& v1, const VariantValue& v2);
VariantValue operator*(const VariantValue& v1, const VariantValue& v2);
VariantValue operator/(const VariantValue& v1, const VariantValue& v2);

bool operator==(const VariantValue& v1, const VariantValue& v2);
bool operator!=(const VariantValue& v1, const VariantValue& v2);
bool operator<(const VariantValue& v1, const VariantValue& v2);
bool operator>(const VariantValue& v1, const VariantValue& v2);
bool operator<=(const VariantValue& v1, const VariantValue& v2);
bool operator>=(const VariantValue& v1, const VariantValue& v2);

NLOHMANN_JSON_SERIALIZE_ENUM(VariantValue::Type, {
	{ VariantValue::Type::type_null, nullptr },
	{ VariantValue::Type::type_bool, "bool" },
	{ VariantValue::Type::type_int, "int" },
	{ VariantValue::Type::type_float, "float" },
	{ VariantValue::Type::type_float2, "float2" },
	{ VariantValue::Type::type_float3, "float3" },
	{ VariantValue::Type::type_float4, "float4" }
})

void to_json(nlohmann::ordered_json& j, const VariantValue& value);
void from_json(const nlohmann::ordered_json& j, VariantValue& value);
}