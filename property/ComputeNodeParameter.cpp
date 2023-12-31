#include "ComputeNodeParameter.h"
#include "../common/JsonUtil.h"

namespace pixelpart {
ComputeNodeParameter::Value ComputeNodeParameter::Value::IntValue(int32_t v) {
	ComputeNodeParameter::Value value;
	value.type = type_int;
	value.data.integer = v;

	return value;
}
ComputeNodeParameter::Value ComputeNodeParameter::Value::FloatValue(floatd v) {
	ComputeNodeParameter::Value value;
	value.type = type_float;
	value.data.numbers[0] = v;

	return value;
}
ComputeNodeParameter::Value ComputeNodeParameter::Value::Float2Value(const vec2d& v) {
	ComputeNodeParameter::Value value;
	value.type = type_float2;
	value.data.numbers[0] = v.x;
	value.data.numbers[1] = v.y;

	return value;
}
ComputeNodeParameter::Value ComputeNodeParameter::Value::Float3Value(const vec3d& v) {
	ComputeNodeParameter::Value value;
	value.type = type_float3;
	value.data.numbers[0] = v.x;
	value.data.numbers[1] = v.y;
	value.data.numbers[2] = v.z;

	return value;
}
ComputeNodeParameter::Value ComputeNodeParameter::Value::Float4Value(const vec4d& v) {
	ComputeNodeParameter::Value value;
	value.type = type_float4;
	value.data.numbers[0] = v.x;
	value.data.numbers[1] = v.y;
	value.data.numbers[2] = v.z;
	value.data.numbers[3] = v.w;

	return value;
}
ComputeNodeParameter::Value ComputeNodeParameter::Value::BoolValue(bool v) {
	ComputeNodeParameter::Value value;
	value.type = type_bool;
	value.data.integer = v ? 1 : 0;

	return value;
}
ComputeNodeParameter::Value ComputeNodeParameter::Value::EnumValue(int32_t v) {
	ComputeNodeParameter::Value value;
	value.type = type_enum;
	value.data.integer = v;

	return value;
}
ComputeNodeParameter::Value ComputeNodeParameter::Value::ColorValue(const vec4d& v) {
	ComputeNodeParameter::Value value;
	value.type = type_color;
	value.data.numbers[0] = v.x;
	value.data.numbers[1] = v.y;
	value.data.numbers[2] = v.z;
	value.data.numbers[3] = v.w;

	return value;
}
ComputeNodeParameter::Value ComputeNodeParameter::Value::CurveValue(const Curve<floatd>& v) {
	ComputeNodeParameter::Value value;
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
ComputeNodeParameter::Value ComputeNodeParameter::Value::GradientValue(const Curve<vec3d>& v) {
	ComputeNodeParameter::Value value;
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
ComputeNodeParameter::Value ComputeNodeParameter::Value::GradientValue(const Curve<vec4d>& v) {
	ComputeNodeParameter::Value value;
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
ComputeNodeParameter::Value ComputeNodeParameter::Value::ImageResourceValue(const std::string& v) {
	ComputeNodeParameter::Value value;
	value.type = type_resource_image;
	value.data.string = v;

	return value;
}
ComputeNodeParameter::Value ComputeNodeParameter::Value::ImageResourceValue(const std::string& id, const std::string& path) {
	ComputeNodeParameter::Value value;
	value.type = type_resource_image;
	value.data.string = id + (!path.empty() ? "@" + path : "");

	return value;
}

ComputeNodeParameter::Value::Value() : type(type_int) {
	data.integer = 0;
}

int32_t ComputeNodeParameter::Value::getInt() const {
	return data.integer;
}
floatd ComputeNodeParameter::Value::getFloat() const {
	return data.numbers[0];
}
vec2d ComputeNodeParameter::Value::getFloat2() const {
	return vec2d(data.numbers[0], data.numbers[1]);
}
vec3d ComputeNodeParameter::Value::getFloat3() const {
	return vec3d(data.numbers[0], data.numbers[1], data.numbers[2]);
}
vec4d ComputeNodeParameter::Value::getFloat4() const {
	return vec4d(data.numbers[0], data.numbers[1], data.numbers[2], data.numbers[3]);
}
bool ComputeNodeParameter::Value::getBool() const {
	return data.integer != 0;
}
int32_t ComputeNodeParameter::Value::getEnum() const {
	return data.integer;
}
vec4d ComputeNodeParameter::Value::getColor() const {
	return vec4d(data.numbers[0], data.numbers[1], data.numbers[2], data.numbers[3]);
}
Curve<floatd> ComputeNodeParameter::Value::getCurve() const {
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
Curve<vec3d> ComputeNodeParameter::Value::getGradient() const {
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
Curve<vec4d> ComputeNodeParameter::Value::getGradient4() const {
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
std::string ComputeNodeParameter::Value::getResourceString() const {
	return data.string;
}
std::string ComputeNodeParameter::Value::getResourceId() const {
	return data.string.substr(0, data.string.find("@"));
}
std::string ComputeNodeParameter::Value::getResourcePath() const {
	std::size_t separatorPos = data.string.find("@");
	return (separatorPos != std::string::npos)
		? data.string.substr(separatorPos + 1)
		: std::string();
}

ComputeNodeParameter ComputeNodeParameter::createIntParameter(const std::string& name, int32_t def, int32_t min, int32_t max) {
	return ComputeNodeParameter{
		name,
		Value::IntValue(def),
		Value::IntValue(min),
		Value::IntValue(max),
		std::vector<std::string>()
	};
}
ComputeNodeParameter ComputeNodeParameter::createFloatParameter(const std::string& name, floatd def, floatd min, floatd max) {
	return ComputeNodeParameter{
		name,
		Value::FloatValue(def),
		Value::FloatValue(min),
		Value::FloatValue(max),
		std::vector<std::string>()
	};
}
ComputeNodeParameter ComputeNodeParameter::createFloat2Parameter(const std::string& name, const vec2d& def, const vec2d& min, const vec2d& max) {
	return ComputeNodeParameter{
		name,
		Value::Float2Value(def),
		Value::Float2Value(min),
		Value::Float2Value(max),
		std::vector<std::string>()
	};
}
ComputeNodeParameter ComputeNodeParameter::createFloat3Parameter(const std::string& name, const vec3d& def, const vec3d& min, const vec3d& max) {
	return ComputeNodeParameter{
		name,
		Value::Float3Value(def),
		Value::Float3Value(min),
		Value::Float3Value(max),
		std::vector<std::string>()
	};
}
ComputeNodeParameter ComputeNodeParameter::createFloat4Parameter(const std::string& name, const vec4d& def, const vec4d& min, const vec4d& max) {
	return ComputeNodeParameter{
		name,
		Value::Float4Value(def),
		Value::Float4Value(min),
		Value::Float4Value(max),
		std::vector<std::string>()
	};
}
ComputeNodeParameter ComputeNodeParameter::createBoolParameter(const std::string& name, bool def) {
	return ComputeNodeParameter{
		name,
		Value::BoolValue(def),
		Value::BoolValue(false),
		Value::BoolValue(true),
		std::vector<std::string>()
	};
}
ComputeNodeParameter ComputeNodeParameter::createEnumParameter(const std::string& name, int32_t def, const std::vector<std::string>& options) {
	return ComputeNodeParameter{
		name,
		Value::EnumValue(std::max(def, 0)),
		Value::EnumValue(0),
		Value::EnumValue(std::max(static_cast<int32_t>(options.size()) - 1, 0)),
		options
	};
}
ComputeNodeParameter ComputeNodeParameter::createColorParameter(const std::string& name, const vec4d& def) {
	return ComputeNodeParameter{
		name,
		Value::ColorValue(def),
		Value::ColorValue(vec4d(0.0)),
		Value::ColorValue(vec4d(1.0)),
		std::vector<std::string>()
	};
}
ComputeNodeParameter ComputeNodeParameter::createCurveParameter(const std::string& name, const Curve<floatd>& def) {
	return ComputeNodeParameter{
		name,
		Value::CurveValue(def),
		Value::CurveValue(Curve<floatd>()),
		Value::CurveValue(Curve<floatd>()),
		std::vector<std::string>()
	};
}
ComputeNodeParameter ComputeNodeParameter::createGradientParameter(const std::string& name, const Curve<vec3d>& def) {
	return ComputeNodeParameter{
		name,
		Value::GradientValue(def),
		Value::GradientValue(Curve<vec3d>()),
		Value::GradientValue(Curve<vec3d>()),
		std::vector<std::string>()
	};
}
ComputeNodeParameter ComputeNodeParameter::createGradientParameter(const std::string& name, const Curve<vec4d>& def) {
	return ComputeNodeParameter{
		name,
		Value::GradientValue(def),
		Value::GradientValue(Curve<vec4d>()),
		Value::GradientValue(Curve<vec4d>()),
		std::vector<std::string>()
	};
}
ComputeNodeParameter ComputeNodeParameter::createImageResourceParameter(const std::string& name) {
	return ComputeNodeParameter{
		name,
		Value::ImageResourceValue(""),
		Value::ImageResourceValue(""),
		Value::ImageResourceValue(""),
		std::vector<std::string>()
	};
}

void to_json(nlohmann::ordered_json& j, const ComputeNodeParameter::Value& value) {
	j = nlohmann::ordered_json{
		{ "type", value.type },
		{ "value", nullptr }
	};

	switch(value.type) {
		case ComputeNodeParameter::Value::type_int:
		case ComputeNodeParameter::Value::type_bool:
		case ComputeNodeParameter::Value::type_enum:
			j["value"] = value.getInt();
			break;
		case ComputeNodeParameter::Value::type_float:
			j["value"] = value.getFloat();
			break;
		case ComputeNodeParameter::Value::type_float2:
			j["value"] = value.getFloat2();
			break;
		case ComputeNodeParameter::Value::type_float3:
			j["value"] = value.getFloat3();
			break;
		case ComputeNodeParameter::Value::type_float4:
		case ComputeNodeParameter::Value::type_color:
			j["value"] = value.getFloat4();
			break;
		case ComputeNodeParameter::Value::type_curve:
			j["value"] = value.getCurve();
			break;
		case ComputeNodeParameter::Value::type_gradient:
			j["value"] = value.getGradient();
			break;
		case ComputeNodeParameter::Value::type_resource_image:
			j["value"] = value.getResourceString();
			break;
		default:
			break;
	}
}
void to_json(nlohmann::ordered_json& j, const ComputeNodeParameter& parameter) {
	j = nlohmann::ordered_json{
		{ "name", parameter.name },
		{ "default_value", parameter.defaultValue },
		{ "min_value", parameter.minValue },
		{ "max_value", parameter.maxValue },
		{ "value_names", parameter.valueNames }
	};
}
void from_json(const nlohmann::ordered_json& j, ComputeNodeParameter::Value& value) {
	ComputeNodeParameter::Value::Type type = j.at("type");

	switch(type) {
		case ComputeNodeParameter::Value::type_int:
			value = ComputeNodeParameter::Value::IntValue(j.at("value").get<int32_t>());
			break;
		case ComputeNodeParameter::Value::type_float:
			value = ComputeNodeParameter::Value::FloatValue(j.at("value").get<floatd>());
			break;
		case ComputeNodeParameter::Value::type_float2:
			value = ComputeNodeParameter::Value::Float2Value(j.at("value").get<vec2d>());
			break;
		case ComputeNodeParameter::Value::type_float3:
			value = ComputeNodeParameter::Value::Float3Value(j.at("value").get<vec3d>());
			break;
		case ComputeNodeParameter::Value::type_float4:
			value = ComputeNodeParameter::Value::Float4Value(j.at("value").get<vec4d>());
			break;
		case ComputeNodeParameter::Value::type_bool:
			value = ComputeNodeParameter::Value::BoolValue(static_cast<bool>(j.at("value").get<int32_t>()));
			break;
		case ComputeNodeParameter::Value::type_enum:
			value = ComputeNodeParameter::Value::EnumValue(j.at("value").get<int32_t>());
			break;
		case ComputeNodeParameter::Value::type_color:
			value = ComputeNodeParameter::Value::ColorValue(j.at("value").get<vec4d>());
			break;
		case ComputeNodeParameter::Value::type_curve:
			value = ComputeNodeParameter::Value::CurveValue(j.at("value").get<Curve<floatd>>());
			break;
		case ComputeNodeParameter::Value::type_gradient:
			value = ComputeNodeParameter::Value::GradientValue(j.at("value").get<Curve<vec3d>>());
			break;
		case ComputeNodeParameter::Value::type_resource_image:
			value = ComputeNodeParameter::Value::ImageResourceValue(j.at("value").get<std::string>());
			break;
		default:
			value = ComputeNodeParameter::Value();
			break;
	}
}
void from_json(const nlohmann::ordered_json& j, ComputeNodeParameter& parameter) {
	parameter = ComputeNodeParameter();

	fromJson(parameter.name, j, "name");
	fromJson(parameter.defaultValue, j, "default_value");
	fromJson(parameter.minValue, j, "min_value");
	fromJson(parameter.maxValue, j, "max_value");
	fromJson(parameter.valueNames, j, "value_names");
}
}