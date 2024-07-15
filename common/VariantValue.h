#pragma once

#include "Types.h"

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
struct VariantTag<int_t> { typedef IntTag Type; };
template <>
struct VariantTag<float_t> { typedef FloatTag Type; };
template <>
struct VariantTag<vec2_t> { typedef Float2Tag Type; };
template <>
struct VariantTag<vec3_t> { typedef Float3Tag Type; };
template <>
struct VariantTag<vec4_t> { typedef Float4Tag Type; };

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
		int_t integer;
		float_t number[4];
	} data;

	static VariantValue Bool(bool v);
	static VariantValue Int(int_t v);
	static VariantValue Float(float_t v);
	static VariantValue Float2(const vec2_t& v);
	static VariantValue Float3(const vec3_t& v);
	static VariantValue Float4(const vec4_t& v);

	VariantValue();
	VariantValue(Type t);

	template <typename T>
	T get() const {
		typedef typename VariantTag<T>::Type TagType;

		return get(TagType());
	}

	bool get(BoolTag t) const;
	int_t get(IntTag t) const;
	float_t get(FloatTag t) const;
	vec2_t get(Float2Tag t) const;
	vec3_t get(Float3Tag t) const;
	vec4_t get(Float4Tag t) const;

	bool getBool() const;
	int_t getInt() const;
	float_t getFloat() const;
	vec2_t getFloat2() const;
	vec3_t getFloat3() const;
	vec4_t getFloat4() const;

	bool toBool() const;
	int_t toInt() const;
	float_t toFloat() const;
	vec2_t toFloat2() const;
	vec3_t toFloat3() const;
	vec4_t toFloat4() const;

	VariantValue cast(Type targetType) const;
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