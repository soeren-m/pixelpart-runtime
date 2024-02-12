#include "VariantValue.h"

namespace pixelpart {
VariantValue VariantValue::Bool(bool v) {
	VariantValue value;
	value.type = type_bool;
	value.data.boolean = v;

	return value;
}
VariantValue VariantValue::Int(int_t v) {
	VariantValue value;
	value.type = type_int;
	value.data.integer = v;

	return value;
}
VariantValue VariantValue::Float(float_t v) {
	VariantValue value;
	value.type = type_float;
	value.data.number[0] = v;

	return value;
}
VariantValue VariantValue::Float2(const vec2_t& v) {
	VariantValue value;
	value.type = type_float2;
	value.data.number[0] = v.x;
	value.data.number[1] = v.y;

	return value;
}
VariantValue VariantValue::Float3(const vec3_t& v) {
	VariantValue value;
	value.type = type_float3;
	value.data.number[0] = v.x;
	value.data.number[1] = v.y;
	value.data.number[2] = v.z;

	return value;
}
VariantValue VariantValue::Float4(const vec4_t& v) {
	VariantValue value;
	value.type = type_float4;
	value.data.number[0] = v.x;
	value.data.number[1] = v.y;
	value.data.number[2] = v.z;
	value.data.number[3] = v.w;

	return value;
}

VariantValue::VariantValue() {

}
VariantValue::VariantValue(Type t) : type(t) {
	switch(type) {
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

bool VariantValue::get(BoolTag t) const {
	return toBool();
}
int_t VariantValue::get(IntTag t) const {
	return toInt();
}
float_t VariantValue::get(FloatTag t) const {
	return toFloat();
}
vec2_t VariantValue::get(Float2Tag t) const {
	return toFloat2();
}
vec3_t VariantValue::get(Float3Tag t) const {
	return toFloat3();
}
vec4_t VariantValue::get(Float4Tag t) const {
	return toFloat4();
}

bool VariantValue::getBool() const {
	return data.boolean;
}
int_t VariantValue::getInt() const {
	return data.integer;
}
float_t VariantValue::getFloat() const {
	return data.number[0];
}
vec2_t VariantValue::getFloat2() const {
	return vec2_t(data.number[0], data.number[1]);
}
vec3_t VariantValue::getFloat3() const {
	return vec3_t(data.number[0], data.number[1], data.number[2]);
}
vec4_t VariantValue::getFloat4() const {
	return vec4_t(data.number[0], data.number[1], data.number[2], data.number[3]);
}

bool VariantValue::toBool() const {
	switch(type) {
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
	switch(type) {
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
	switch(type) {
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
vec2_t VariantValue::toFloat2() const {
	switch(type) {
		case type_bool:
			return vec2_t(data.boolean ? 1.0 : 0.0);
		case type_int:
			return vec2_t(static_cast<float_t>(data.integer));
		case type_float:
			return vec2_t(data.number[0]);
		case type_float2:
		case type_float3:
		case type_float4:
			return vec2_t(data.number[0], data.number[1]);
		default:
			return vec2_t(0.0);
	}
}
vec3_t VariantValue::toFloat3() const {
	switch(type) {
		case type_bool:
			return vec3_t(data.boolean ? 1.0 : 0.0);
		case type_int:
			return vec3_t(static_cast<float_t>(data.integer));
		case type_float:
		case type_float2:
			return vec3_t(data.number[0]);
		case type_float3:
		case type_float4:
			return vec3_t(data.number[0], data.number[1], data.number[2]);
		default:
			return vec3_t(0.0);
	}
}
vec4_t VariantValue::toFloat4() const {
	switch(type) {
		case type_bool:
			return vec4_t(data.boolean ? 1.0 : 0.0);
		case type_int:
			return vec4_t(static_cast<float_t>(data.integer));
		case type_float:
		case type_float2:
		case type_float3:
			return vec4_t(data.number[0]);
		case type_float4:
			return vec4_t(data.number[0], data.number[1], data.number[2], data.number[3]);
		default:
			return vec4_t(0.0);
	}
}

VariantValue VariantValue::cast(Type type) const {
	switch(type) {
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

VariantValue operator+(const VariantValue& v1, const VariantValue& v2) {
	if(v1.type == VariantValue::type_null || v2.type == VariantValue::type_null) {
		return VariantValue();
	}

	switch(v1.type) {
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
	if(v1.type == VariantValue::type_null || v2.type == VariantValue::type_null) {
		return VariantValue();
	}

	switch(v1.type) {
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
	if(v1.type == VariantValue::type_null || v2.type == VariantValue::type_null) {
		return VariantValue();
	}

	switch(v1.type) {
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
	if(v1.type == VariantValue::type_null || v2.type == VariantValue::type_null) {
		return VariantValue();
	}

	switch(v1.type) {
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
	if(v1.type != v2.type) {
		return false;
	}

	switch(v1.type) {
		case VariantValue::type_bool:
			return v1.getBool() == v2.getBool();
		case VariantValue::type_int:
			return v1.getInt() == v2.getInt();
		case VariantValue::type_float:
			return v1.getFloat() == v2.getFloat();
		case VariantValue::type_float2:
			return v1.getFloat2() == v2.getFloat2();
		case VariantValue::type_float3:
			return v1.getFloat3() == v2.getFloat3();
		case VariantValue::type_float4:
			return v1.getFloat4() == v2.getFloat4();
		default:
			return true;
	}
}
bool operator!=(const VariantValue& v1, const VariantValue& v2) {
	return !operator==(v1, v2);
}
bool operator<(const VariantValue& v1, const VariantValue& v2) {
	if(v1.type != v2.type) {
		return v1.type < v2.type;
	}

	switch(v1.type) {
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
		{ "type", value.type },
		{ "value", nullptr }
	};

	switch(value.type) {
		case VariantValue::type_bool:
			j["value"] = value.getBool();
			break;
		case VariantValue::type_int:
			j["value"] = value.getInt();
			break;
		case VariantValue::type_float:
			j["value"] = value.getFloat();
			break;
		case VariantValue::type_float2:
			j["value"] = value.getFloat2();
			break;
		case VariantValue::type_float3:
			j["value"] = value.getFloat3();
			break;
		case VariantValue::type_float4:
			j["value"] = value.getFloat4();
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
			value = VariantValue::Float2(j.at("value").get<vec2_t>());
			break;
		case VariantValue::type_float3:
			value = VariantValue::Float3(j.at("value").get<vec3_t>());
			break;
		case VariantValue::type_float4:
			value = VariantValue::Float4(j.at("value").get<vec4_t>());
			break;
		default:
			value = VariantValue();
			break;
	}
}
}