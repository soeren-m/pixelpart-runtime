#include "ShaderParameter.h"
#include "JSONUtil.h"

namespace pixelpart {
ShaderParameter::Value ShaderParameter::Value::IntValue(int32_t v) {
	ShaderParameter::Value value;
	value.type = type_int;
	value.data.integer = v;

	return value;
}
ShaderParameter::Value ShaderParameter::Value::FloatValue(floatd v) {
	ShaderParameter::Value value;
	value.type = type_float;
	value.data.numbers[0] = v;

	return value;
}
ShaderParameter::Value ShaderParameter::Value::Float2Value(const vec2d& v) {
	ShaderParameter::Value value;
	value.type = type_float2;
	value.data.numbers[0] = v.x;
	value.data.numbers[1] = v.y;

	return value;
}
ShaderParameter::Value ShaderParameter::Value::Float3Value(const vec3d& v) {
	ShaderParameter::Value value;
	value.type = type_float3;
	value.data.numbers[0] = v.x;
	value.data.numbers[1] = v.y;
	value.data.numbers[2] = v.z;

	return value;
}
ShaderParameter::Value ShaderParameter::Value::Float4Value(const vec4d& v) {
	ShaderParameter::Value value;
	value.type = type_float4;
	value.data.numbers[0] = v.x;
	value.data.numbers[1] = v.y;
	value.data.numbers[2] = v.z;
	value.data.numbers[3] = v.w;

	return value;
}
ShaderParameter::Value ShaderParameter::Value::BoolValue(bool v) {
	ShaderParameter::Value value;
	value.type = type_bool;
	value.data.integer = v ? 1 : 0;

	return value;
}
ShaderParameter::Value ShaderParameter::Value::EnumValue(int32_t v) {
	ShaderParameter::Value value;
	value.type = type_enum;
	value.data.integer = v;

	return value;
}
ShaderParameter::Value ShaderParameter::Value::ColorValue(const vec4d& v) {
	ShaderParameter::Value value;
	value.type = type_color;
	value.data.numbers[0] = v.x;
	value.data.numbers[1] = v.y;
	value.data.numbers[2] = v.z;
	value.data.numbers[3] = v.w;

	return value;
}
ShaderParameter::Value ShaderParameter::Value::CurveValue(const Curve<floatd>& v) {
	ShaderParameter::Value value;
	value.type = type_curve;
	value.data.integer = static_cast<int32_t>(v.getInterpolation());

	for(std::size_t i = 0; i < 64; i++) {
		if(i < v.getNumPoints()) {
			const Curve<floatd>::Point& point = v.getPoint(i);

			value.data.numbers[i * 2 + 0] = point.position;
			value.data.numbers[i * 2 + 1] = point.value;
		}
		else {
			value.data.numbers[i * 2 + 0] = -1.0;
			value.data.numbers[i * 2 + 1] = -1.0;
		}
	}

	return value;
}
ShaderParameter::Value ShaderParameter::Value::PathValue(const Curve<vec2d>& v) {
	ShaderParameter::Value value;
	value.type = type_path;
	value.data.integer = static_cast<int32_t>(v.getInterpolation());

	for(std::size_t i = 0; i < 64; i++) {
		if(i < v.getNumPoints()) {
			const Curve<vec2d>::Point& point = v.getPoint(i);

			value.data.numbers[i * 3 + 0] = point.position;
			value.data.numbers[i * 3 + 1] = point.value.x;
			value.data.numbers[i * 3 + 2] = point.value.y;
		}
		else {
			value.data.numbers[i * 3 + 0] = -1.0;
			value.data.numbers[i * 3 + 1] = -1.0;
			value.data.numbers[i * 3 + 2] = -1.0;
		}
	}

	return value;
}
ShaderParameter::Value ShaderParameter::Value::GradientValue(const Curve<vec3d>& v) {
	ShaderParameter::Value value;
	value.type = type_gradient;
	value.data.integer = static_cast<int32_t>(v.getInterpolation());

	for(std::size_t i = 0; i < 64; i++) {
		if(i < v.getNumPoints()) {
			const Curve<vec3d>::Point& point = v.getPoint(i);

			value.data.numbers[i * 4 + 0] = point.position;
			value.data.numbers[i * 4 + 1] = point.value.x;
			value.data.numbers[i * 4 + 2] = point.value.y;
			value.data.numbers[i * 4 + 3] = point.value.z;
		}
		else {
			value.data.numbers[i * 4 + 0] = -1.0;
			value.data.numbers[i * 4 + 1] = -1.0;
			value.data.numbers[i * 4 + 2] = -1.0;
			value.data.numbers[i * 4 + 3] = -1.0;
		}
	}

	return value;
}
ShaderParameter::Value ShaderParameter::Value::GradientValue(const Curve<vec4d>& v) {
	ShaderParameter::Value value;
	value.type = type_gradient;
	value.data.integer = static_cast<int32_t>(v.getInterpolation());

	for(std::size_t i = 0; i < 64; i++) {
		if(i < v.getNumPoints()) {
			const Curve<vec4d>::Point& point = v.getPoint(i);

			value.data.numbers[i * 4 + 0] = point.position;
			value.data.numbers[i * 4 + 1] = point.value.x;
			value.data.numbers[i * 4 + 2] = point.value.y;
			value.data.numbers[i * 4 + 3] = point.value.z;
		}
		else {
			value.data.numbers[i * 4 + 0] = -1.0;
			value.data.numbers[i * 4 + 1] = -1.0;
			value.data.numbers[i * 4 + 2] = -1.0;
			value.data.numbers[i * 4 + 3] = -1.0;
		}
	}

	return value;
}
ShaderParameter::Value ShaderParameter::Value::ImageResourceValue(const std::string& v) {
	ShaderParameter::Value value;
	value.type = type_resource_image;
	value.data.string = v;

	return value;
}
ShaderParameter::Value ShaderParameter::Value::ImageResourceValue(const std::string& id, const std::string& path) {
	ShaderParameter::Value value;
	value.type = type_resource_image;
	value.data.string = id + (!path.empty() ? "@" + path : "");

	return value;
}

ShaderParameter::Value::Value() : type(type_int) {
	data.integer = 0;
}

int32_t ShaderParameter::Value::getInt() const {
	return data.integer;
}
floatd ShaderParameter::Value::getFloat() const {
	return data.numbers[0];
}
vec2d ShaderParameter::Value::getFloat2() const {
	return vec2d(data.numbers[0], data.numbers[1]);
}
vec3d ShaderParameter::Value::getFloat3() const {
	return vec3d(data.numbers[0], data.numbers[1], data.numbers[2]);
}
vec4d ShaderParameter::Value::getFloat4() const {
	return vec4d(data.numbers[0], data.numbers[1], data.numbers[2], data.numbers[3]);
}
bool ShaderParameter::Value::getBool() const {
	return data.integer != 0;
}
int32_t ShaderParameter::Value::getEnum() const {
	return data.integer;
}
vec4d ShaderParameter::Value::getColor() const {
	return vec4d(data.numbers[0], data.numbers[1], data.numbers[2], data.numbers[3]);
}
Curve<floatd> ShaderParameter::Value::getCurve() const {
	std::vector<Curve<floatd>::Point> points;
	for(std::size_t i = 0; i < 64; i++) {
		if(data.numbers[i * 2] < 0.0) {
			break;
		}

		points.push_back(Curve<floatd>::Point{
			data.numbers[i * 2 + 0],
			data.numbers[i * 2 + 1]
		});
	}

	return Curve<floatd>(points, static_cast<CurveInterpolation>(data.integer));
}
Curve<vec2d> ShaderParameter::Value::getPath() const {
	std::vector<Curve<vec2d>::Point> points;
	for(std::size_t i = 0; i < 64; i++) {
		if(data.numbers[i * 3] < 0.0) {
			break;
		}

		points.push_back(Curve<vec2d>::Point{
			data.numbers[i * 3 + 0],
			vec2d(
				data.numbers[i * 3 + 1],
				data.numbers[i * 3 + 2])
		});
	}

	return Curve<vec2d>(points, static_cast<CurveInterpolation>(data.integer));
}
Curve<vec3d> ShaderParameter::Value::getGradient() const {
	std::vector<Curve<vec3d>::Point> points;
	for(std::size_t i = 0; i < 64; i++) {
		if(data.numbers[i * 4] < 0.0) {
			break;
		}

		points.push_back(Curve<vec3d>::Point{
			data.numbers[i * 4 + 0],
			vec3d(
				data.numbers[i * 4 + 1],
				data.numbers[i * 4 + 2],
				data.numbers[i * 4 + 3])
		});
	}

	return Curve<vec3d>(points, static_cast<CurveInterpolation>(data.integer));
}
Curve<vec4d> ShaderParameter::Value::getGradient4() const {
	std::vector<Curve<vec4d>::Point> points;
	for(std::size_t i = 0; i < 64; i++) {
		if(data.numbers[i * 4] < 0.0) {
			break;
		}

		points.push_back(Curve<vec4d>::Point{
			data.numbers[i * 4 + 0],
			vec4d(
				data.numbers[i * 4 + 1],
				data.numbers[i * 4 + 2],
				data.numbers[i * 4 + 3],
				1.0)
		});
	}

	return Curve<vec4d>(points, static_cast<CurveInterpolation>(data.integer));
}
std::string ShaderParameter::Value::getResourceString() const {
	return data.string;
}
std::string ShaderParameter::Value::getResourceId() const {
	return data.string.substr(0, data.string.find("@"));
}
std::string ShaderParameter::Value::getResourcePath() const {
	std::size_t separatorPos = data.string.find("@");
	return (separatorPos != std::string::npos)
		? data.string.substr(separatorPos + 1)
		: std::string();
}

ShaderParameter ShaderParameter::createIntParameter(const std::string& name, int32_t def, int32_t min, int32_t max) {
	return ShaderParameter{
		name,
		Value::IntValue(def),
		Value::IntValue(min),
		Value::IntValue(max),
		std::vector<std::string>()
	};
}
ShaderParameter ShaderParameter::createFloatParameter(const std::string& name, floatd def, floatd min, floatd max) {
	return ShaderParameter{
		name,
		Value::FloatValue(def),
		Value::FloatValue(min),
		Value::FloatValue(max),
		std::vector<std::string>()
	};
}
ShaderParameter ShaderParameter::createFloat2Parameter(const std::string& name, const vec2d& def, const vec2d& min, const vec2d& max) {
	return ShaderParameter{
		name,
		Value::Float2Value(def),
		Value::Float2Value(min),
		Value::Float2Value(max),
		std::vector<std::string>()
	};
}
ShaderParameter ShaderParameter::createFloat3Parameter(const std::string& name, const vec3d& def, const vec3d& min, const vec3d& max) {
	return ShaderParameter{
		name,
		Value::Float3Value(def),
		Value::Float3Value(min),
		Value::Float3Value(max),
		std::vector<std::string>()
	};
}
ShaderParameter ShaderParameter::createFloat4Parameter(const std::string& name, const vec4d& def, const vec4d& min, const vec4d& max) {
	return ShaderParameter{
		name,
		Value::Float4Value(def),
		Value::Float4Value(min),
		Value::Float4Value(max),
		std::vector<std::string>()
	};
}
ShaderParameter ShaderParameter::createBoolParameter(const std::string& name, bool def) {
	return ShaderParameter{
		name,
		Value::BoolValue(def),
		Value::BoolValue(false),
		Value::BoolValue(true),
		std::vector<std::string>()
	};
}
ShaderParameter ShaderParameter::createEnumParameter(const std::string& name, int32_t def, const std::vector<std::string>& options) {
	return ShaderParameter{
		name,
		Value::EnumValue(std::max(def, 0)),
		Value::EnumValue(0),
		Value::EnumValue(std::max(static_cast<int32_t>(options.size()) - 1, 0)),
		options
	};
}
ShaderParameter ShaderParameter::createColorParameter(const std::string& name, const vec4d& def) {
	return ShaderParameter{
		name,
		Value::ColorValue(def),
		Value::ColorValue(vec4d(0.0)),
		Value::ColorValue(vec4d(1.0)),
		std::vector<std::string>()
	};
}
ShaderParameter ShaderParameter::createCurveParameter(const std::string& name, const Curve<floatd>& def) {
	return ShaderParameter{
		name,
		Value::CurveValue(def),
		Value::CurveValue(Curve<floatd>()),
		Value::CurveValue(Curve<floatd>()),
		std::vector<std::string>()
	};
}
ShaderParameter ShaderParameter::createPathParameter(const std::string& name, const Curve<vec2d>& def) {
	return ShaderParameter{
		name,
		Value::PathValue(def),
		Value::PathValue(Curve<vec2d>()),
		Value::PathValue(Curve<vec2d>()),
		std::vector<std::string>()
	};
}
ShaderParameter ShaderParameter::createGradientParameter(const std::string& name, const Curve<vec3d>& def) {
	return ShaderParameter{
		name,
		Value::GradientValue(def),
		Value::GradientValue(Curve<vec3d>()),
		Value::GradientValue(Curve<vec3d>()),
		std::vector<std::string>()
	};
}
ShaderParameter ShaderParameter::createImageResourceParameter(const std::string& name) {
	return ShaderParameter{
		name,
		Value::ImageResourceValue(""),
		Value::ImageResourceValue(""),
		Value::ImageResourceValue(""),
		std::vector<std::string>()
	};
}

void to_json(nlohmann::ordered_json& j, const ShaderParameter::Value& value) {
	j = nlohmann::ordered_json{
		{ "type", value.type },
		{ "value", nullptr }
	};

	switch(value.type) {
		case ShaderParameter::Value::type_int:
		case ShaderParameter::Value::type_bool:
		case ShaderParameter::Value::type_enum:
			j["value"] = value.getInt();
			break;
		case ShaderParameter::Value::type_float:
			j["value"] = value.getFloat();
			break;
		case ShaderParameter::Value::type_float2:
			j["value"] = value.getFloat2();
			break;
		case ShaderParameter::Value::type_float3:
			j["value"] = value.getFloat3();
			break;
		case ShaderParameter::Value::type_float4:
		case ShaderParameter::Value::type_color:
			j["value"] = value.getFloat4();
			break;
		case ShaderParameter::Value::type_curve:
			j["value"] = value.getCurve();
			break;
		case ShaderParameter::Value::type_path:
			j["value"] = value.getPath();
			break;
		case ShaderParameter::Value::type_gradient:
			j["value"] = value.getGradient();
			break;
		case ShaderParameter::Value::type_resource_image:
			j["value"] = value.getResourceString();
			break;
		default:
			break;
	}
}
void to_json(nlohmann::ordered_json& j, const ShaderParameter& parameter) {
	j = nlohmann::ordered_json{
		{ "name", parameter.name },
		{ "default_value", parameter.defaultValue },
		{ "min_value", parameter.minValue },
		{ "max_value", parameter.maxValue },
		{ "value_names", parameter.valueNames }
	};
}
void from_json(const nlohmann::ordered_json& j, ShaderParameter::Value& value) {
	ShaderParameter::Value::Type type = j.at("type");

	switch(type) {
		case ShaderParameter::Value::type_int:
			value = ShaderParameter::Value::IntValue(j.at("value").get<int32_t>());
			break;
		case ShaderParameter::Value::type_float:
			value = ShaderParameter::Value::FloatValue(j.at("value").get<floatd>());
			break;
		case ShaderParameter::Value::type_float2:
			value = ShaderParameter::Value::Float2Value(j.at("value").get<vec2d>());
			break;
		case ShaderParameter::Value::type_float3:
			value = ShaderParameter::Value::Float3Value(j.at("value").get<vec3d>());
			break;
		case ShaderParameter::Value::type_float4:
			value = ShaderParameter::Value::Float4Value(j.at("value").get<vec4d>());
			break;
		case ShaderParameter::Value::type_bool:
			value = ShaderParameter::Value::BoolValue(static_cast<bool>(j.at("value").get<int32_t>()));
			break;
		case ShaderParameter::Value::type_enum:
			value = ShaderParameter::Value::EnumValue(j.at("value").get<int32_t>());
			break;
		case ShaderParameter::Value::type_color:
			value = ShaderParameter::Value::ColorValue(j.at("value").get<vec4d>());
			break;
		case ShaderParameter::Value::type_curve:
			value = ShaderParameter::Value::CurveValue(j.at("value").get<Curve<floatd>>());
			break;
		case ShaderParameter::Value::type_path:
			value = ShaderParameter::Value::PathValue(j.at("value").get<Curve<vec2d>>());
			break;
		case ShaderParameter::Value::type_gradient:
			value = ShaderParameter::Value::GradientValue(j.at("value").get<Curve<vec3d>>());
			break;
		case ShaderParameter::Value::type_resource_image:
			value = ShaderParameter::Value::ImageResourceValue(j.at("value").get<std::string>());
			break;
		default:
			value = ShaderParameter::Value();
			break;
	}
}
void from_json(const nlohmann::ordered_json& j, ShaderParameter& parameter) {
	parameter = ShaderParameter();
	fromJson(parameter.name, j, "name", "");
	fromJson(parameter.defaultValue, j, "default_value", "");
	fromJson(parameter.minValue, j, "min_value", "");
	fromJson(parameter.maxValue, j, "max_value", "");
	fromJson(parameter.valueNames, j, "value_names", "");
}
}