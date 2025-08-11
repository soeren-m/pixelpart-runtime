#include "VariantValue.h"

namespace pixelpart {
VariantValue VariantValue::Bool(bool v) {
	VariantValue value;
	value.dataType = type_bool;
	value.data.boolean = v;

	return value;
}
VariantValue VariantValue::Int(int_t v) {
	VariantValue value;
	value.dataType = type_int;
	value.data.integer = v;

	return value;
}
VariantValue VariantValue::Float(float_t v) {
	VariantValue value;
	value.dataType = type_float;
	value.data.number[0] = v;

	return value;
}
VariantValue VariantValue::Float2(const float2_t& v) {
	VariantValue value;
	value.dataType = type_float2;
	value.data.number[0] = v.x;
	value.data.number[1] = v.y;

	return value;
}
VariantValue VariantValue::Float3(const float3_t& v) {
	VariantValue value;
	value.dataType = type_float3;
	value.data.number[0] = v.x;
	value.data.number[1] = v.y;
	value.data.number[2] = v.z;

	return value;
}
VariantValue VariantValue::Float4(const float4_t& v) {
	VariantValue value;
	value.dataType = type_float4;
	value.data.number[0] = v.x;
	value.data.number[1] = v.y;
	value.data.number[2] = v.z;
	value.data.number[3] = v.w;

	return value;
}

VariantValue::VariantValue(Type varType) : dataType(varType) {
	switch(varType) {
		case type_bool:
			data.boolean = false;
			break;
		case type_int:
			data.integer = 0;
			break;
		case type_float:
		case type_float2:
		case type_float3:
		case type_float4:
			data.number[0] = 0.0;
			data.number[1] = 0.0;
			data.number[2] = 0.0;
			data.number[3] = 0.0;
			break;
		default:
			break;
	}
}

VariantValue::Type VariantValue::type() const {
	return dataType;
}

bool VariantValue::valueBool() const {
	return data.boolean;
}
int_t VariantValue::valueInt() const {
	return data.integer;
}
float_t VariantValue::valueFloat() const {
	return data.number[0];
}
float2_t VariantValue::valueFloat2() const {
	return float2_t(data.number[0], data.number[1]);
}
float3_t VariantValue::valueFloat3() const {
	return float3_t(data.number[0], data.number[1], data.number[2]);
}
float4_t VariantValue::valueFloat4() const {
	return float4_t(data.number[0], data.number[1], data.number[2], data.number[3]);
}

bool VariantValue::toBool() const {
	switch(dataType) {
		case type_bool:
			return data.boolean;
		case type_int:
			return data.integer != 0;
		case type_float:
		case type_float2:
		case type_float3:
		case type_float4:
			return data.number[0] != 0.0;
		default:
			return false;
	}
}
int_t VariantValue::toInt() const {
	switch(dataType) {
		case type_bool:
			return data.boolean ? 1 : 0;
		case type_int:
			return data.integer;
		case type_float:
		case type_float2:
		case type_float3:
		case type_float4:
			return static_cast<int_t>(data.number[0]);
		default:
			return 0;
	}
}
float_t VariantValue::toFloat() const {
	switch(dataType) {
		case type_bool:
			return data.boolean ? 1.0 : 0.0;
		case type_int:
			return static_cast<float_t>(data.integer);
		case type_float:
		case type_float2:
		case type_float3:
		case type_float4:
			return data.number[0];
		default:
			return 0.0;
	}
}
float2_t VariantValue::toFloat2() const {
	switch(dataType) {
		case type_bool:
			return float2_t(data.boolean ? 1.0 : 0.0);
		case type_int:
			return float2_t(static_cast<float_t>(data.integer));
		case type_float:
			return float2_t(data.number[0]);
		case type_float2:
		case type_float3:
		case type_float4:
			return float2_t(data.number[0], data.number[1]);
		default:
			return float2_t(0.0);
	}
}
float3_t VariantValue::toFloat3() const {
	switch(dataType) {
		case type_bool:
			return float3_t(data.boolean ? 1.0 : 0.0);
		case type_int:
			return float3_t(static_cast<float_t>(data.integer));
		case type_float:
		case type_float2:
			return float3_t(data.number[0]);
		case type_float3:
		case type_float4:
			return float3_t(data.number[0], data.number[1], data.number[2]);
		default:
			return float3_t(0.0);
	}
}
float4_t VariantValue::toFloat4() const {
	switch(dataType) {
		case type_bool:
			return float4_t(data.boolean ? 1.0 : 0.0);
		case type_int:
			return float4_t(static_cast<float_t>(data.integer));
		case type_float:
		case type_float2:
		case type_float3:
			return float4_t(data.number[0]);
		case type_float4:
			return float4_t(data.number[0], data.number[1], data.number[2], data.number[3]);
		default:
			return float4_t(0.0);
	}
}

VariantValue VariantValue::cast(Type targetType) const {
	switch(targetType) {
		case type_bool:
			return VariantValue::Bool(toBool());
		case type_int:
			return VariantValue::Int(toInt());
		case type_float:
			return VariantValue::Float(toFloat());
		case type_float2:
			return VariantValue::Float2(toFloat2());
		case type_float3:
			return VariantValue::Float3(toFloat3());
		case type_float4:
			return VariantValue::Float4(toFloat4());
		default:
			return VariantValue();
	}
}

bool VariantValue::value(BoolTag t) const {
	return toBool();
}
int_t VariantValue::value(IntTag t) const {
	return toInt();
}
float_t VariantValue::value(FloatTag t) const {
	return toFloat();
}
float2_t VariantValue::value(Float2Tag t) const {
	return toFloat2();
}
float3_t VariantValue::value(Float3Tag t) const {
	return toFloat3();
}
float4_t VariantValue::value(Float4Tag t) const {
	return toFloat4();
}

VariantValue operator+(const VariantValue& v1, const VariantValue& v2) {
	if(v1.type() == VariantValue::type_null || v2.type() == VariantValue::type_null) {
		return VariantValue();
	}

	switch(v1.type()) {
		case VariantValue::type_int:
			return VariantValue::Int(v1.toInt() + v2.toInt());
		case VariantValue::type_float:
			return VariantValue::Float(v1.toFloat() + v2.toFloat());
		case VariantValue::type_float2:
			return VariantValue::Float2(v1.toFloat2() + v2.toFloat2());
		case VariantValue::type_float3:
			return VariantValue::Float3(v1.toFloat3() + v2.toFloat3());
		case VariantValue::type_float4:
			return VariantValue::Float4(v1.toFloat4() + v2.toFloat4());
		default:
			return VariantValue();
	}
}
VariantValue operator-(const VariantValue& v1, const VariantValue& v2) {
	if(v1.type() == VariantValue::type_null || v2.type() == VariantValue::type_null) {
		return VariantValue();
	}

	switch(v1.type()) {
		case VariantValue::type_int:
			return VariantValue::Int(v1.toInt() - v2.toInt());
		case VariantValue::type_float:
			return VariantValue::Float(v1.toFloat() - v2.toFloat());
		case VariantValue::type_float2:
			return VariantValue::Float2(v1.toFloat2() - v2.toFloat2());
		case VariantValue::type_float3:
			return VariantValue::Float3(v1.toFloat3() - v2.toFloat3());
		case VariantValue::type_float4:
			return VariantValue::Float4(v1.toFloat4() - v2.toFloat4());
		default:
			return VariantValue();
	}
}
VariantValue operator*(const VariantValue& v1, const VariantValue& v2) {
	if(v1.type() == VariantValue::type_null || v2.type() == VariantValue::type_null) {
		return VariantValue();
	}

	switch(v1.type()) {
		case VariantValue::type_int:
			return VariantValue::Int(v1.toInt() * v2.toInt());
		case VariantValue::type_float:
			return VariantValue::Float(v1.toFloat() * v2.toFloat());
		case VariantValue::type_float2:
			return VariantValue::Float2(v1.toFloat2() * v2.toFloat2());
		case VariantValue::type_float3:
			return VariantValue::Float3(v1.toFloat3() * v2.toFloat3());
		case VariantValue::type_float4:
			return VariantValue::Float4(v1.toFloat4() * v2.toFloat4());
		default:
			return VariantValue();
	}
}
VariantValue operator/(const VariantValue& v1, const VariantValue& v2) {
	if(v1.type() == VariantValue::type_null || v2.type() == VariantValue::type_null) {
		return VariantValue();
	}

	switch(v1.type()) {
		case VariantValue::type_int:
			return VariantValue::Int(v1.toInt() / v2.toInt());
		case VariantValue::type_float:
			return VariantValue::Float(v1.toFloat() / v2.toFloat());
		case VariantValue::type_float2:
			return VariantValue::Float2(v1.toFloat2() / v2.toFloat2());
		case VariantValue::type_float3:
			return VariantValue::Float3(v1.toFloat3() / v2.toFloat3());
		case VariantValue::type_float4:
			return VariantValue::Float4(v1.toFloat4() / v2.toFloat4());
		default:
			return VariantValue();
	}
}

bool operator==(const VariantValue& v1, const VariantValue& v2) {
	if(v1.type() != v2.type()) {
		return false;
	}

	switch(v1.type()) {
		case VariantValue::type_bool:
			return v1.valueBool() == v2.valueBool();
		case VariantValue::type_int:
			return v1.valueInt() == v2.valueInt();
		case VariantValue::type_float:
			return v1.valueFloat() == v2.valueFloat();
		case VariantValue::type_float2:
			return v1.valueFloat2() == v2.valueFloat2();
		case VariantValue::type_float3:
			return v1.valueFloat3() == v2.valueFloat3();
		case VariantValue::type_float4:
			return v1.valueFloat4() == v2.valueFloat4();
		default:
			return true;
	}
}
bool operator!=(const VariantValue& v1, const VariantValue& v2) {
	return !operator==(v1, v2);
}
bool operator<(const VariantValue& v1, const VariantValue& v2) {
	if(v1.type() != v2.type()) {
		return v1.type() < v2.type();
	}

	switch(v1.type()) {
		case VariantValue::type_bool:
		case VariantValue::type_int:
			return v1.toInt() < v2.toInt();
		case VariantValue::type_float:
		case VariantValue::type_float2:
		case VariantValue::type_float3:
		case VariantValue::type_float4:
			return v1.toFloat() < v2.toFloat();
		default:
			return true;
	}
}
bool operator>(const VariantValue& v1, const VariantValue& v2) {
	return operator<(v2, v1);
}
bool operator<=(const VariantValue& v1, const VariantValue& v2) {
	return !operator>(v1, v2);
}
bool operator>=(const VariantValue& v1, const VariantValue& v2) {
	return !operator<(v1, v2);
}

void to_json(nlohmann::ordered_json& j, const VariantValue& value) {
	j = nlohmann::ordered_json{
		{ "type", value.type() },
		{ "value", nullptr }
	};

	switch(value.type()) {
		case VariantValue::type_bool:
			j["value"] = value.valueBool();
			break;
		case VariantValue::type_int:
			j["value"] = value.valueInt();
			break;
		case VariantValue::type_float:
			j["value"] = value.valueFloat();
			break;
		case VariantValue::type_float2:
			j["value"] = value.valueFloat2();
			break;
		case VariantValue::type_float3:
			j["value"] = value.valueFloat3();
			break;
		case VariantValue::type_float4:
			j["value"] = value.valueFloat4();
			break;
		default:
			break;
	}
}
void from_json(const nlohmann::ordered_json& j, VariantValue& value) {
	VariantValue::Type type = j.at("type");

	switch(type) {
		case VariantValue::type_bool:
			value = VariantValue::Bool(j.at("value").get<bool>());
			break;
		case VariantValue::type_int:
			value = VariantValue::Int(j.at("value").get<int_t>());
			break;
		case VariantValue::type_float:
			value = VariantValue::Float(j.at("value").get<float_t>());
			break;
		case VariantValue::type_float2:
			value = VariantValue::Float2(j.at("value").get<float2_t>());
			break;
		case VariantValue::type_float3:
			value = VariantValue::Float3(j.at("value").get<float3_t>());
			break;
		case VariantValue::type_float4:
			value = VariantValue::Float4(j.at("value").get<float4_t>());
			break;
		default:
			value = VariantValue();
			break;
	}
}
}
