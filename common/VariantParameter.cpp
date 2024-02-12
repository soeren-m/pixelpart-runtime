#include "VariantParameter.h"
#include "../common/JsonUtil.h"

namespace pixelpart {
VariantParameter::Value VariantParameter::Value::IntValue(int_t v) {
	VariantParameter::Value value;
	value.type = type_int;
	value.data.integer = v;

	return value;
}
VariantParameter::Value VariantParameter::Value::FloatValue(float_t v) {
	VariantParameter::Value value;
	value.type = type_float;
	value.data.numbers[0] = v;

	return value;
}
VariantParameter::Value VariantParameter::Value::Float2Value(const vec2_t& v) {
	VariantParameter::Value value;
	value.type = type_float2;
	value.data.numbers[0] = v.x;
	value.data.numbers[1] = v.y;

	return value;
}
VariantParameter::Value VariantParameter::Value::Float3Value(const vec3_t& v) {
	VariantParameter::Value value;
	value.type = type_float3;
	value.data.numbers[0] = v.x;
	value.data.numbers[1] = v.y;
	value.data.numbers[2] = v.z;

	return value;
}
VariantParameter::Value VariantParameter::Value::Float4Value(const vec4_t& v) {
	VariantParameter::Value value;
	value.type = type_float4;
	value.data.numbers[0] = v.x;
	value.data.numbers[1] = v.y;
	value.data.numbers[2] = v.z;
	value.data.numbers[3] = v.w;

	return value;
}
VariantParameter::Value VariantParameter::Value::BoolValue(bool v) {
	VariantParameter::Value value;
	value.type = type_bool;
	value.data.integer = v ? 1 : 0;

	return value;
}
VariantParameter::Value VariantParameter::Value::EnumValue(int_t v) {
	VariantParameter::Value value;
	value.type = type_enum;
	value.data.integer = v;

	return value;
}
VariantParameter::Value VariantParameter::Value::ColorValue(const vec4_t& v) {
	VariantParameter::Value value;
	value.type = type_color;
	value.data.numbers[0] = v.x;
	value.data.numbers[1] = v.y;
	value.data.numbers[2] = v.z;
	value.data.numbers[3] = v.w;

	return value;
}
VariantParameter::Value VariantParameter::Value::CurveValue(const Curve<float_t>& v) {
	VariantParameter::Value value;
	value.type = type_curve;
	value.data.integer = static_cast<int_t>(v.getInterpolation());

	for(std::size_t i = 0u; i < 64u; i++) {
		if(i < v.getNumPoints()) {
			const Curve<float_t>::Point& point = v.getPoint(i);

			value.data.numbers[i * 2u + 0u] = point.position;
			value.data.numbers[i * 2u + 1u] = point.value;
		}
		else {
			value.data.numbers[i * 2u + 0u] = -1.0;
			value.data.numbers[i * 2u + 1u] = -1.0;
		}
	}

	return value;
}
VariantParameter::Value VariantParameter::Value::GradientValue(const Curve<vec3_t>& v) {
	VariantParameter::Value value;
	value.type = type_gradient;
	value.data.integer = static_cast<int_t>(v.getInterpolation());

	for(std::size_t i = 0u; i < 64u; i++) {
		if(i < v.getNumPoints()) {
			const Curve<vec3_t>::Point& point = v.getPoint(i);

			value.data.numbers[i * 4u + 0u] = point.position;
			value.data.numbers[i * 4u + 1u] = point.value.x;
			value.data.numbers[i * 4u + 2u] = point.value.y;
			value.data.numbers[i * 4u + 3u] = point.value.z;
		}
		else {
			value.data.numbers[i * 4u + 0u] = -1.0;
			value.data.numbers[i * 4u + 1u] = -1.0;
			value.data.numbers[i * 4u + 2u] = -1.0;
			value.data.numbers[i * 4u + 3u] = -1.0;
		}
	}

	return value;
}
VariantParameter::Value VariantParameter::Value::GradientValue(const Curve<vec4_t>& v) {
	VariantParameter::Value value;
	value.type = type_gradient;
	value.data.integer = static_cast<int_t>(v.getInterpolation());

	for(std::size_t i = 0u; i < 64u; i++) {
		if(i < v.getNumPoints()) {
			const Curve<vec4_t>::Point& point = v.getPoint(i);

			value.data.numbers[i * 4u + 0u] = point.position;
			value.data.numbers[i * 4u + 1u] = point.value.x;
			value.data.numbers[i * 4u + 2u] = point.value.y;
			value.data.numbers[i * 4u + 3u] = point.value.z;
		}
		else {
			value.data.numbers[i * 4u + 0u] = -1.0;
			value.data.numbers[i * 4u + 1u] = -1.0;
			value.data.numbers[i * 4u + 2u] = -1.0;
			value.data.numbers[i * 4u + 3u] = -1.0;
		}
	}

	return value;
}
VariantParameter::Value VariantParameter::Value::ImageResourceValue(const std::string& v) {
	VariantParameter::Value value;
	value.type = type_resource_image;
	value.data.string = v;

	return value;
}
VariantParameter::Value VariantParameter::Value::ImageResourceValue(const std::string& id, const std::string& path) {
	VariantParameter::Value value;
	value.type = type_resource_image;
	value.data.string = id + (!path.empty() ? "@" + path : "");

	return value;
}

VariantParameter::Value::Value() : type(type_int) {
	data.integer = 0;
}

int_t VariantParameter::Value::getInt() const {
	return data.integer;
}
float_t VariantParameter::Value::getFloat() const {
	return data.numbers[0];
}
vec2_t VariantParameter::Value::getFloat2() const {
	return vec2_t(data.numbers[0], data.numbers[1]);
}
vec3_t VariantParameter::Value::getFloat3() const {
	return vec3_t(data.numbers[0], data.numbers[1], data.numbers[2]);
}
vec4_t VariantParameter::Value::getFloat4() const {
	return vec4_t(data.numbers[0], data.numbers[1], data.numbers[2], data.numbers[3]);
}
bool VariantParameter::Value::getBool() const {
	return data.integer != 0;
}
int_t VariantParameter::Value::getEnum() const {
	return data.integer;
}
vec4_t VariantParameter::Value::getColor() const {
	return vec4_t(data.numbers[0], data.numbers[1], data.numbers[2], data.numbers[3]);
}
Curve<float_t> VariantParameter::Value::getCurve() const {
	std::vector<Curve<float_t>::Point> points;
	for(std::size_t i = 0u; i < 64u; i++) {
		if(data.numbers[i * 2u] < 0.0) {
			break;
		}

		points.push_back(Curve<float_t>::Point{
			data.numbers[i * 2u + 0u],
			data.numbers[i * 2u + 1u]
		});
	}

	return Curve<float_t>(points, static_cast<CurveInterpolation>(data.integer));
}
Curve<vec3_t> VariantParameter::Value::getGradient() const {
	std::vector<Curve<vec3_t>::Point> points;
	for(std::size_t i = 0u; i < 64u; i++) {
		if(data.numbers[i * 4u] < 0.0) {
			break;
		}

		points.push_back(Curve<vec3_t>::Point{
			data.numbers[i * 4u + 0u],
			vec3_t(
				data.numbers[i * 4u + 1u],
				data.numbers[i * 4u + 2u],
				data.numbers[i * 4u + 3u])
		});
	}

	return Curve<vec3_t>(points, static_cast<CurveInterpolation>(data.integer));
}
Curve<vec4_t> VariantParameter::Value::getGradient4() const {
	std::vector<Curve<vec4_t>::Point> points;
	for(std::size_t i = 0u; i < 64u; i++) {
		if(data.numbers[i * 4u] < 0.0) {
			break;
		}

		points.push_back(Curve<vec4_t>::Point{
			data.numbers[i * 4 + 0],
			vec4_t(
				data.numbers[i * 4u + 1u],
				data.numbers[i * 4u + 2u],
				data.numbers[i * 4u + 3u],
				1.0)
		});
	}

	return Curve<vec4_t>(points, static_cast<CurveInterpolation>(data.integer));
}
std::string VariantParameter::Value::getResourceString() const {
	return data.string;
}
std::string VariantParameter::Value::getResourceId() const {
	return data.string.substr(0u, data.string.find("@"));
}
std::string VariantParameter::Value::getResourcePath() const {
	std::size_t separatorPos = data.string.find("@");
	return (separatorPos != std::string::npos)
		? data.string.substr(separatorPos + 1u)
		: std::string();
}

VariantParameter VariantParameter::createIntParameter(const std::string& name, int_t def, int_t min, int_t max) {
	return VariantParameter{
		name,
		Value::IntValue(def),
		Value::IntValue(min),
		Value::IntValue(max),
		std::vector<std::string>()
	};
}
VariantParameter VariantParameter::createFloatParameter(const std::string& name, float_t def, float_t min, float_t max) {
	return VariantParameter{
		name,
		Value::FloatValue(def),
		Value::FloatValue(min),
		Value::FloatValue(max),
		std::vector<std::string>()
	};
}
VariantParameter VariantParameter::createFloat2Parameter(const std::string& name, const vec2_t& def, const vec2_t& min, const vec2_t& max) {
	return VariantParameter{
		name,
		Value::Float2Value(def),
		Value::Float2Value(min),
		Value::Float2Value(max),
		std::vector<std::string>()
	};
}
VariantParameter VariantParameter::createFloat3Parameter(const std::string& name, const vec3_t& def, const vec3_t& min, const vec3_t& max) {
	return VariantParameter{
		name,
		Value::Float3Value(def),
		Value::Float3Value(min),
		Value::Float3Value(max),
		std::vector<std::string>()
	};
}
VariantParameter VariantParameter::createFloat4Parameter(const std::string& name, const vec4_t& def, const vec4_t& min, const vec4_t& max) {
	return VariantParameter{
		name,
		Value::Float4Value(def),
		Value::Float4Value(min),
		Value::Float4Value(max),
		std::vector<std::string>()
	};
}
VariantParameter VariantParameter::createBoolParameter(const std::string& name, bool def) {
	return VariantParameter{
		name,
		Value::BoolValue(def),
		Value::BoolValue(false),
		Value::BoolValue(true),
		std::vector<std::string>()
	};
}
VariantParameter VariantParameter::createEnumParameter(const std::string& name, int_t def, const std::vector<std::string>& options) {
	return VariantParameter{
		name,
		Value::EnumValue(std::max(def, static_cast<int_t>(0))),
		Value::EnumValue(0),
		Value::EnumValue(std::max(static_cast<int_t>(options.size()) - 1, static_cast<int_t>(0))),
		options
	};
}
VariantParameter VariantParameter::createColorParameter(const std::string& name, const vec4_t& def) {
	return VariantParameter{
		name,
		Value::ColorValue(def),
		Value::ColorValue(vec4_t(0.0)),
		Value::ColorValue(vec4_t(1.0)),
		std::vector<std::string>()
	};
}
VariantParameter VariantParameter::createCurveParameter(const std::string& name, const Curve<float_t>& def) {
	return VariantParameter{
		name,
		Value::CurveValue(def),
		Value::CurveValue(Curve<float_t>()),
		Value::CurveValue(Curve<float_t>()),
		std::vector<std::string>()
	};
}
VariantParameter VariantParameter::createGradientParameter(const std::string& name, const Curve<vec3_t>& def) {
	return VariantParameter{
		name,
		Value::GradientValue(def),
		Value::GradientValue(Curve<vec3_t>()),
		Value::GradientValue(Curve<vec3_t>()),
		std::vector<std::string>()
	};
}
VariantParameter VariantParameter::createGradientParameter(const std::string& name, const Curve<vec4_t>& def) {
	return VariantParameter{
		name,
		Value::GradientValue(def),
		Value::GradientValue(Curve<vec4_t>()),
		Value::GradientValue(Curve<vec4_t>()),
		std::vector<std::string>()
	};
}
VariantParameter VariantParameter::createImageResourceParameter(const std::string& name) {
	return VariantParameter{
		name,
		Value::ImageResourceValue(""),
		Value::ImageResourceValue(""),
		Value::ImageResourceValue(""),
		std::vector<std::string>()
	};
}

void to_json(nlohmann::ordered_json& j, const VariantParameter::Value& value) {
	j = nlohmann::ordered_json{
		{ "type", value.type },
		{ "value", nullptr }
	};

	switch(value.type) {
		case VariantParameter::Value::type_int:
		case VariantParameter::Value::type_bool:
		case VariantParameter::Value::type_enum:
			j["value"] = value.getInt();
			break;
		case VariantParameter::Value::type_float:
			j["value"] = value.getFloat();
			break;
		case VariantParameter::Value::type_float2:
			j["value"] = value.getFloat2();
			break;
		case VariantParameter::Value::type_float3:
			j["value"] = value.getFloat3();
			break;
		case VariantParameter::Value::type_float4:
		case VariantParameter::Value::type_color:
			j["value"] = value.getFloat4();
			break;
		case VariantParameter::Value::type_curve:
			j["value"] = value.getCurve();
			break;
		case VariantParameter::Value::type_gradient:
			j["value"] = value.getGradient();
			break;
		case VariantParameter::Value::type_resource_image:
			j["value"] = value.getResourceString();
			break;
		default:
			break;
	}
}
void to_json(nlohmann::ordered_json& j, const VariantParameter& parameter) {
	j = nlohmann::ordered_json{
		{ "name", parameter.name },
		{ "default_value", parameter.defaultValue },
		{ "min_value", parameter.minValue },
		{ "max_value", parameter.maxValue },
		{ "value_names", parameter.valueNames }
	};
}
void from_json(const nlohmann::ordered_json& j, VariantParameter::Value& value) {
	VariantParameter::Value::Type type = j.at("type");

	switch(type) {
		case VariantParameter::Value::type_int:
			value = VariantParameter::Value::IntValue(j.at("value").get<int_t>());
			break;
		case VariantParameter::Value::type_float:
			value = VariantParameter::Value::FloatValue(j.at("value").get<float_t>());
			break;
		case VariantParameter::Value::type_float2:
			value = VariantParameter::Value::Float2Value(j.at("value").get<vec2_t>());
			break;
		case VariantParameter::Value::type_float3:
			value = VariantParameter::Value::Float3Value(j.at("value").get<vec3_t>());
			break;
		case VariantParameter::Value::type_float4:
			value = VariantParameter::Value::Float4Value(j.at("value").get<vec4_t>());
			break;
		case VariantParameter::Value::type_bool:
			value = VariantParameter::Value::BoolValue(static_cast<bool>(j.at("value").get<int_t>()));
			break;
		case VariantParameter::Value::type_enum:
			value = VariantParameter::Value::EnumValue(j.at("value").get<int_t>());
			break;
		case VariantParameter::Value::type_color:
			value = VariantParameter::Value::ColorValue(j.at("value").get<vec4_t>());
			break;
		case VariantParameter::Value::type_curve:
			value = VariantParameter::Value::CurveValue(j.at("value").get<Curve<float_t>>());
			break;
		case VariantParameter::Value::type_gradient:
			value = VariantParameter::Value::GradientValue(j.at("value").get<Curve<vec3_t>>());
			break;
		case VariantParameter::Value::type_resource_image:
			value = VariantParameter::Value::ImageResourceValue(j.at("value").get<std::string>());
			break;
		default:
			value = VariantParameter::Value();
			break;
	}
}
void from_json(const nlohmann::ordered_json& j, VariantParameter& parameter) {
	parameter = VariantParameter();

	fromJson(parameter.name, j, "name");
	fromJson(parameter.defaultValue, j, "default_value");
	fromJson(parameter.minValue, j, "min_value");
	fromJson(parameter.maxValue, j, "max_value");
	fromJson(parameter.valueNames, j, "value_names");
}
}