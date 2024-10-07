#pragma once

#include "Types.h"
#include "Math.h"
#include "../json/json.hpp"

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
struct VariantTag<float2_t> { typedef Float2Tag Type; };
template <>
struct VariantTag<float3_t> { typedef Float3Tag Type; };
template <>
struct VariantTag<float4_t> { typedef Float4Tag Type; };

class VariantValue {
public:
	enum Type : int32_t {
		type_null = -1,
		type_bool,
		type_int,
		type_float,
		type_float2,
		type_float3,
		type_float4
	};

	static VariantValue Bool(bool v);
	static VariantValue Int(int_t v);
	static VariantValue Float(float_t v);
	static VariantValue Float2(const float2_t& v);
	static VariantValue Float3(const float3_t& v);
	static VariantValue Float4(const float4_t& v);

	VariantValue() = default;
	VariantValue(Type varType);

	Type type() const;

	template <typename T>
	T value() const {
		typedef typename VariantTag<T>::Type TagType;

		return value(TagType());
	}

	bool valueBool() const;
	int_t valueInt() const;
	float_t valueFloat() const;
	float2_t valueFloat2() const;
	float3_t valueFloat3() const;
	float4_t valueFloat4() const;

	bool toBool() const;
	int_t toInt() const;
	float_t toFloat() const;
	float2_t toFloat2() const;
	float3_t toFloat3() const;
	float4_t toFloat4() const;

	VariantValue cast(Type targetType) const;

private:
	bool value(BoolTag t) const;
	int_t value(IntTag t) const;
	float_t value(FloatTag t) const;
	float2_t value(Float2Tag t) const;
	float3_t value(Float3Tag t) const;
	float4_t value(Float4Tag t) const;

	Type dataType = type_null;

	union {
		bool boolean;
		int_t integer;
		float_t number[4];
	} data;
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