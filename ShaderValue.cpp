#include "ShaderValue.h"

namespace pixelpart {
ShaderValue ShaderValue::BoolValue(bool v) {
	ShaderValue value;
	value.type = type_bool;
	value.data.boolean = v;

	return value;
}
ShaderValue ShaderValue::IntValue(int32_t v) {
	ShaderValue value;
	value.type = type_int;
	value.data.integer = v;

	return value;
}
ShaderValue ShaderValue::FloatValue(floatd v) {
	ShaderValue value;
	value.type = type_float;
	value.data.number[0] = v;

	return value;
}
ShaderValue ShaderValue::Float2Value(const vec2d& v) {
	ShaderValue value;
	value.type = type_float2;
	value.data.number[0] = v.x;
	value.data.number[1] = v.y;

	return value;
}
ShaderValue ShaderValue::Float3Value(const vec3d& v) {
	ShaderValue value;
	value.type = type_float3;
	value.data.number[0] = v.x;
	value.data.number[1] = v.y;
	value.data.number[2] = v.z;

	return value;
}
ShaderValue ShaderValue::Float4Value(const vec4d& v) {
	ShaderValue value;
	value.type = type_float4;
	value.data.number[0] = v.x;
	value.data.number[1] = v.y;
	value.data.number[2] = v.z;
	value.data.number[3] = v.w;

	return value;
}

ShaderValue::ShaderValue() {

}

bool ShaderValue::getBool() const {
	return data.boolean;
}
int32_t ShaderValue::getInt() const {
	return data.integer;
}
floatd ShaderValue::getFloat() const {
	return data.number[0];
}
vec2d ShaderValue::getFloat2() const {
	return vec2d(data.number[0], data.number[1]);
}
vec3d ShaderValue::getFloat3() const {
	return vec3d(data.number[0], data.number[1], data.number[2]);
}
vec4d ShaderValue::getFloat4() const {
	return vec4d(data.number[0], data.number[1], data.number[2], data.number[3]);
}

void to_json(nlohmann::ordered_json& j, const ShaderValue& value) {
	j = nlohmann::ordered_json{
		{ "type", value.type },
		{ "value", nullptr }
	};

	switch(value.type) {
		case ShaderValue::type_bool:
			j["value"] = value.getBool();
			break;
		case ShaderValue::type_int:
			j["value"] = value.getInt();
			break;
		case ShaderValue::type_float:
			j["value"] = value.getFloat();
			break;
		case ShaderValue::type_float2:
			j["value"] = value.getFloat2();
			break;
		case ShaderValue::type_float3:
			j["value"] = value.getFloat3();
			break;
		case ShaderValue::type_float4:
			j["value"] = value.getFloat4();
			break;
		default:
			break;
	}
}
void from_json(const nlohmann::ordered_json& j, ShaderValue& value) {
	ShaderValue::Type type = j.at("type");

	switch(type) {
		case ShaderValue::type_bool:
			value = ShaderValue::BoolValue(j.at("value").get<bool>());
			break;
		case ShaderValue::type_int:
			value = ShaderValue::IntValue(j.at("value").get<int32_t>());
			break;
		case ShaderValue::type_float:
			value = ShaderValue::FloatValue(j.at("value").get<floatd>());
			break;
		case ShaderValue::type_float2:
			value = ShaderValue::Float2Value(j.at("value").get<vec2d>());
			break;
		case ShaderValue::type_float3:
			value = ShaderValue::Float3Value(j.at("value").get<vec3d>());
			break;
		case ShaderValue::type_float4:
			value = ShaderValue::Float4Value(j.at("value").get<vec4d>());
			break;
		default:
			value = ShaderValue();
			break;
	}
}
}