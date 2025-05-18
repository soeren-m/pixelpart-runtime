#include "VariantParameter.h"
#include <algorithm>

namespace pixelpart {
VariantParameter::Value VariantParameter::Value::IntValue(int_t v) {
	VariantParameter::Value value;
	value.valueType = type_int;
	value.data.integer = v;

	return value;
}
VariantParameter::Value VariantParameter::Value::FloatValue(float_t v) {
	VariantParameter::Value value;
	value.valueType = type_float;
	value.data.numbers[0] = v;

	return value;
}
VariantParameter::Value VariantParameter::Value::Float2Value(const float2_t& v) {
	VariantParameter::Value value;
	value.valueType = type_float2;
	value.data.numbers[0] = v.x;
	value.data.numbers[1] = v.y;

	return value;
}
VariantParameter::Value VariantParameter::Value::Float3Value(const float3_t& v) {
	VariantParameter::Value value;
	value.valueType = type_float3;
	value.data.numbers[0] = v.x;
	value.data.numbers[1] = v.y;
	value.data.numbers[2] = v.z;

	return value;
}
VariantParameter::Value VariantParameter::Value::Float4Value(const float4_t& v) {
	VariantParameter::Value value;
	value.valueType = type_float4;
	value.data.numbers[0] = v.x;
	value.data.numbers[1] = v.y;
	value.data.numbers[2] = v.z;
	value.data.numbers[3] = v.w;

	return value;
}
VariantParameter::Value VariantParameter::Value::BoolValue(bool v) {
	VariantParameter::Value value;
	value.valueType = type_bool;
	value.data.integer = v ? 1 : 0;

	return value;
}
VariantParameter::Value VariantParameter::Value::EnumValue(int_t v) {
	VariantParameter::Value value;
	value.valueType = type_enum;
	value.data.integer = v;

	return value;
}
VariantParameter::Value VariantParameter::Value::ColorValue(const float4_t& v) {
	VariantParameter::Value value;
	value.valueType = type_color;
	value.data.numbers[0] = v.x;
	value.data.numbers[1] = v.y;
	value.data.numbers[2] = v.z;
	value.data.numbers[3] = v.w;

	return value;
}
VariantParameter::Value VariantParameter::Value::CurveValue(const Curve<float_t>& v) {
	VariantParameter::Value value;
	value.valueType = type_curve;
	value.data.integer = static_cast<int_t>(v.interpolation());

	for(std::size_t pointIndex = 0; pointIndex < 64; pointIndex++) {
		if(pointIndex < v.pointCount()) {
			const Curve<float_t>::Point& point = v.point(pointIndex);

			value.data.numbers[pointIndex * 2 + 0] = point.position;
			value.data.numbers[pointIndex * 2 + 1] = point.value;
		}
		else {
			value.data.numbers[pointIndex * 2 + 0] = -1.0;
			value.data.numbers[pointIndex * 2 + 1] = -1.0;
		}
	}

	return value;
}
VariantParameter::Value VariantParameter::Value::GradientValue(const Curve<float3_t>& v) {
	VariantParameter::Value value;
	value.valueType = type_gradient;
	value.data.integer = static_cast<int_t>(v.interpolation());

	for(std::size_t pointIndex = 0; pointIndex < 64; pointIndex++) {
		if(pointIndex < v.pointCount()) {
			const Curve<float3_t>::Point& point = v.point(pointIndex);

			value.data.numbers[pointIndex * 4 + 0] = point.position;
			value.data.numbers[pointIndex * 4 + 1] = point.value.x;
			value.data.numbers[pointIndex * 4 + 2] = point.value.y;
			value.data.numbers[pointIndex * 4 + 3] = point.value.z;
		}
		else {
			value.data.numbers[pointIndex * 4 + 0] = -1.0;
			value.data.numbers[pointIndex * 4 + 1] = -1.0;
			value.data.numbers[pointIndex * 4 + 2] = -1.0;
			value.data.numbers[pointIndex * 4 + 3] = -1.0;
		}
	}

	return value;
}
VariantParameter::Value VariantParameter::Value::GradientValue(const Curve<float4_t>& v) {
	VariantParameter::Value value;
	value.valueType = type_gradient;
	value.data.integer = static_cast<int_t>(v.interpolation());

	for(std::size_t pointIndex = 0; pointIndex < 64; pointIndex++) {
		if(pointIndex < v.pointCount()) {
			const Curve<float4_t>::Point& point = v.point(pointIndex);

			value.data.numbers[pointIndex * 4 + 0] = point.position;
			value.data.numbers[pointIndex * 4 + 1] = point.value.x;
			value.data.numbers[pointIndex * 4 + 2] = point.value.y;
			value.data.numbers[pointIndex * 4 + 3] = point.value.z;
		}
		else {
			value.data.numbers[pointIndex * 4 + 0] = -1.0;
			value.data.numbers[pointIndex * 4 + 1] = -1.0;
			value.data.numbers[pointIndex * 4 + 2] = -1.0;
			value.data.numbers[pointIndex * 4 + 3] = -1.0;
		}
	}

	return value;
}
VariantParameter::Value VariantParameter::Value::ImageResourceValue(const std::string& v) {
	VariantParameter::Value value;
	value.valueType = type_resource_image;
	value.data.string = v;

	return value;
}

VariantParameter::Value::Type VariantParameter::Value::type() const {
	return valueType;
}

int_t VariantParameter::Value::valueInt() const {
	return data.integer;
}
float_t VariantParameter::Value::valueFloat() const {
	return data.numbers[0];
}
float2_t VariantParameter::Value::valueFloat2() const {
	return float2_t(data.numbers[0], data.numbers[1]);
}
float3_t VariantParameter::Value::valueFloat3() const {
	return float3_t(data.numbers[0], data.numbers[1], data.numbers[2]);
}
float4_t VariantParameter::Value::valueFloat4() const {
	return float4_t(data.numbers[0], data.numbers[1], data.numbers[2], data.numbers[3]);
}
bool VariantParameter::Value::valueBool() const {
	return data.integer != 0;
}
int_t VariantParameter::Value::valueEnum() const {
	return data.integer;
}
float4_t VariantParameter::Value::valueColor() const {
	return float4_t(data.numbers[0], data.numbers[1], data.numbers[2], data.numbers[3]);
}
Curve<float_t> VariantParameter::Value::valueCurve() const {
	std::vector<Curve<float_t>::Point> points;
	for(std::size_t pointIndex = 0; pointIndex < 64; pointIndex++) {
		if(data.numbers[pointIndex * 2] < 0.0) {
			break;
		}

		points.push_back(Curve<float_t>::Point{
			data.numbers[pointIndex * 2 + 0],
			data.numbers[pointIndex * 2 + 1]
		});
	}

	return Curve<float_t>(points, static_cast<CurveInterpolation>(data.integer));
}
Curve<float3_t> VariantParameter::Value::valueGradient() const {
	std::vector<Curve<float3_t>::Point> points;
	for(std::size_t pointIndex = 0; pointIndex < 64; pointIndex++) {
		if(data.numbers[pointIndex * 4] < 0.0) {
			break;
		}

		points.push_back(Curve<float3_t>::Point{
			data.numbers[pointIndex * 4 + 0],
			float3_t(
				data.numbers[pointIndex * 4 + 1],
				data.numbers[pointIndex * 4 + 2],
				data.numbers[pointIndex * 4 + 3])
		});
	}

	return Curve<float3_t>(points, static_cast<CurveInterpolation>(data.integer));
}
Curve<float4_t> VariantParameter::Value::valueGradient4() const {
	std::vector<Curve<float4_t>::Point> points;
	for(std::size_t pointIndex = 0; pointIndex < 64; pointIndex++) {
		if(data.numbers[pointIndex * 4] < 0.0) {
			break;
		}

		points.push_back(Curve<float4_t>::Point{
			data.numbers[pointIndex * 4 + 0],
			float4_t(
				data.numbers[pointIndex * 4 + 1],
				data.numbers[pointIndex * 4 + 2],
				data.numbers[pointIndex * 4 + 3],
				1.0)
		});
	}

	return Curve<float4_t>(points, static_cast<CurveInterpolation>(data.integer));
}
std::string VariantParameter::Value::valueResourceId() const {
	return data.string;
}

VariantParameter VariantParameter::IntParameter(const std::string& name, int_t def, int_t min, int_t max) {
	return VariantParameter{
		name,
		Value::IntValue(def),
		Value::IntValue(min),
		Value::IntValue(max),
		std::vector<std::string>()
	};
}
VariantParameter VariantParameter::FloatParameter(const std::string& name, float_t def, float_t min, float_t max) {
	return VariantParameter{
		name,
		Value::FloatValue(def),
		Value::FloatValue(min),
		Value::FloatValue(max),
		std::vector<std::string>()
	};
}
VariantParameter VariantParameter::Float2Parameter(const std::string& name, const float2_t& def, const float2_t& min, const float2_t& max) {
	return VariantParameter{
		name,
		Value::Float2Value(def),
		Value::Float2Value(min),
		Value::Float2Value(max),
		std::vector<std::string>()
	};
}
VariantParameter VariantParameter::Float3Parameter(const std::string& name, const float3_t& def, const float3_t& min, const float3_t& max) {
	return VariantParameter{
		name,
		Value::Float3Value(def),
		Value::Float3Value(min),
		Value::Float3Value(max),
		std::vector<std::string>()
	};
}
VariantParameter VariantParameter::Float4Parameter(const std::string& name, const float4_t& def, const float4_t& min, const float4_t& max) {
	return VariantParameter{
		name,
		Value::Float4Value(def),
		Value::Float4Value(min),
		Value::Float4Value(max),
		std::vector<std::string>()
	};
}
VariantParameter VariantParameter::BoolParameter(const std::string& name, bool def) {
	return VariantParameter{
		name,
		Value::BoolValue(def),
		Value::BoolValue(false),
		Value::BoolValue(true),
		std::vector<std::string>()
	};
}
VariantParameter VariantParameter::EnumParameter(const std::string& name, int_t def, const std::vector<std::string>& options) {
	return VariantParameter{
		name,
		Value::EnumValue(std::max(def, static_cast<int_t>(0))),
		Value::EnumValue(0),
		Value::EnumValue(std::max(static_cast<int_t>(options.size()) - 1, static_cast<int_t>(0))),
		options
	};
}
VariantParameter VariantParameter::ColorParameter(const std::string& name, const float4_t& def) {
	return VariantParameter{
		name,
		Value::ColorValue(def),
		Value::ColorValue(float4_t(0.0)),
		Value::ColorValue(float4_t(1.0)),
		std::vector<std::string>()
	};
}
VariantParameter VariantParameter::CurveParameter(const std::string& name, const Curve<float_t>& def) {
	return VariantParameter{
		name,
		Value::CurveValue(def),
		Value::CurveValue(Curve<float_t>()),
		Value::CurveValue(Curve<float_t>()),
		std::vector<std::string>()
	};
}
VariantParameter VariantParameter::GradientParameter(const std::string& name, const Curve<float3_t>& def) {
	return VariantParameter{
		name,
		Value::GradientValue(def),
		Value::GradientValue(Curve<float3_t>()),
		Value::GradientValue(Curve<float3_t>()),
		std::vector<std::string>()
	};
}
VariantParameter VariantParameter::GradientParameter(const std::string& name, const Curve<float4_t>& def) {
	return VariantParameter{
		name,
		Value::GradientValue(def),
		Value::GradientValue(Curve<float4_t>()),
		Value::GradientValue(Curve<float4_t>()),
		std::vector<std::string>()
	};
}
VariantParameter VariantParameter::ImageResourceParameter(const std::string& name) {
	return VariantParameter{
		name,
		Value::ImageResourceValue(""),
		Value::ImageResourceValue(""),
		Value::ImageResourceValue(""),
		std::vector<std::string>()
	};
}

VariantParameter::VariantParameter(const std::string& paramName,
	const Value& paramDefault, const Value& paramMin, const Value& paramMax,
	const std::vector<std::string>& paramValueNames) :
	parameterName(paramName), defaultValue(paramDefault), minValue(paramMin), maxValue(paramMax), valueNames(paramValueNames) {

}

void VariantParameter::name(const std::string& name) {
	parameterName = name;
}
const std::string& VariantParameter::name() const {
	return parameterName;
}

void VariantParameter::def(const Value& value) {
	defaultValue = value;
}
const VariantParameter::Value& VariantParameter::def() const {
	return defaultValue;
}

void VariantParameter::min(const Value& value) {
	minValue = value;
}
const VariantParameter::Value& VariantParameter::min() const {
	return minValue;
}

void VariantParameter::max(const Value& value) {
	maxValue = value;
}
const VariantParameter::Value& VariantParameter::max() const {
	return maxValue;
}

std::vector<std::string>& VariantParameter::options() {
	return valueNames;
}
const std::vector<std::string>& VariantParameter::options() const {
	return valueNames;
}

void to_json(nlohmann::ordered_json& j, const VariantParameter::Value& value) {
	j = nlohmann::ordered_json{
		{ "type", value.type() },
		{ "value", nullptr }
	};

	switch(value.type()) {
		case VariantParameter::Value::type_int:
		case VariantParameter::Value::type_bool:
		case VariantParameter::Value::type_enum:
			j["value"] = value.valueInt();
			break;
		case VariantParameter::Value::type_float:
			j["value"] = value.valueFloat();
			break;
		case VariantParameter::Value::type_float2:
			j["value"] = value.valueFloat2();
			break;
		case VariantParameter::Value::type_float3:
			j["value"] = value.valueFloat3();
			break;
		case VariantParameter::Value::type_float4:
		case VariantParameter::Value::type_color:
			j["value"] = value.valueFloat4();
			break;
		case VariantParameter::Value::type_curve:
			j["value"] = value.valueCurve();
			break;
		case VariantParameter::Value::type_gradient:
			j["value"] = value.valueGradient();
			break;
		case VariantParameter::Value::type_resource_image:
			j["value"] = value.valueResourceId();
			break;
		default:
			break;
	}
}
void to_json(nlohmann::ordered_json& j, const VariantParameter& parameter) {
	j = nlohmann::ordered_json{
		{ "name", parameter.name() },
		{ "default_value", parameter.def() },
		{ "min_value", parameter.min() },
		{ "max_value", parameter.max() },
		{ "value_names", parameter.options() }
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
			value = VariantParameter::Value::Float2Value(j.at("value").get<float2_t>());
			break;
		case VariantParameter::Value::type_float3:
			value = VariantParameter::Value::Float3Value(j.at("value").get<float3_t>());
			break;
		case VariantParameter::Value::type_float4:
			value = VariantParameter::Value::Float4Value(j.at("value").get<float4_t>());
			break;
		case VariantParameter::Value::type_bool:
			value = VariantParameter::Value::BoolValue(static_cast<bool>(j.at("value").get<int_t>()));
			break;
		case VariantParameter::Value::type_enum:
			value = VariantParameter::Value::EnumValue(j.at("value").get<int_t>());
			break;
		case VariantParameter::Value::type_color:
			value = VariantParameter::Value::ColorValue(j.at("value").get<float4_t>());
			break;
		case VariantParameter::Value::type_curve:
			value = VariantParameter::Value::CurveValue(j.at("value").get<Curve<float_t>>());
			break;
		case VariantParameter::Value::type_gradient:
			value = VariantParameter::Value::GradientValue(j.at("value").get<Curve<float3_t>>());
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
	parameter = VariantParameter(
		j.at("name"),
		j.value("default_value", VariantParameter::Value()),
		j.value("min_value", VariantParameter::Value()),
		j.value("max_value", VariantParameter::Value()),
		j.value("value_names", std::vector<std::string>()));
}
}