#pragma once

#include "Types.h"
#include "Math.h"
#include "Curve.h"
#include "../json/json.hpp"
#include <string>
#include <vector>

namespace pixelpart {
class VariantParameter {
public:
	class Value {
	public:
		enum Type : std::uint32_t {
			type_int,
			type_float,
			type_float2,
			type_float3,
			type_float4,
			type_bool,
			type_enum,
			type_color,
			type_curve,
			type_gradient,
			type_resource_image
		};

		static Value IntValue(int_t v);
		static Value FloatValue(float_t v);
		static Value Float2Value(const float2_t& v);
		static Value Float3Value(const float3_t& v);
		static Value Float4Value(const float4_t& v);
		static Value BoolValue(bool v);
		static Value EnumValue(int_t v);
		static Value ColorValue(const float4_t& v);
		static Value CurveValue(const Curve<float_t>& v);
		static Value GradientValue(const Curve<float3_t>& v);
		static Value GradientValue(const Curve<float4_t>& v);
		static Value ImageResourceValue(const std::string& v);

		Type type() const;

		int_t valueInt() const;
		float_t valueFloat() const;
		float2_t valueFloat2() const;
		float3_t valueFloat3() const;
		float4_t valueFloat4() const;
		bool valueBool() const;
		int_t valueEnum() const;
		float4_t valueColor() const;
		Curve<float_t> valueCurve() const;
		Curve<float3_t> valueGradient() const;
		Curve<float4_t> valueGradient4() const;
		std::string valueResourceId() const;

	private:
		struct Data {
			int_t integer = 0;
			float_t numbers[256] = { 0.0 };
			std::string string;
		};

		Type valueType = type_int;
		Data data;
	};

	static VariantParameter IntParameter(const std::string& name, int_t def, int_t min, int_t max);
	static VariantParameter FloatParameter(const std::string& name, float_t def, float_t min, float_t max);
	static VariantParameter Float2Parameter(const std::string& name, const float2_t& def, const float2_t& min, const float2_t& max);
	static VariantParameter Float3Parameter(const std::string& name, const float3_t& def, const float3_t& min, const float3_t& max);
	static VariantParameter Float4Parameter(const std::string& name, const float4_t& def, const float4_t& min, const float4_t& max);
	static VariantParameter BoolParameter(const std::string& name, bool def);
	static VariantParameter EnumParameter(const std::string& name, int_t def, const std::vector<std::string>& options);
	static VariantParameter ColorParameter(const std::string& name, const float4_t& def);
	static VariantParameter CurveParameter(const std::string& name, const Curve<float_t>& def);
	static VariantParameter GradientParameter(const std::string& name, const Curve<float3_t>& def);
	static VariantParameter GradientParameter(const std::string& name, const Curve<float4_t>& def);
	static VariantParameter ImageResourceParameter(const std::string& name);

	VariantParameter() = default;
	VariantParameter(const std::string& paramName,
		const Value& paramDefault, const Value& paramMin, const Value& paramMax,
		const std::vector<std::string>& paramValueNames);

	void name(const std::string& name);
	const std::string& name() const;

	void def(const Value& value);
	const Value& def() const;

	void min(const Value& value);
	const Value& min() const;

	void max(const Value& value);
	const Value& max() const;

	std::vector<std::string>& options();
	const std::vector<std::string>& options() const;

private:
	std::string parameterName;
	Value defaultValue;
	Value minValue;
	Value maxValue;
	std::vector<std::string> valueNames;
};

NLOHMANN_JSON_SERIALIZE_ENUM(VariantParameter::Value::Type, {
	{ VariantParameter::Value::type_int, "int" },
	{ VariantParameter::Value::type_float, "float" },
	{ VariantParameter::Value::type_float2, "float2" },
	{ VariantParameter::Value::type_float3, "float3" },
	{ VariantParameter::Value::type_float4, "float4" },
	{ VariantParameter::Value::type_bool, "bool" },
	{ VariantParameter::Value::type_enum, "enum" },
	{ VariantParameter::Value::type_color, "color" },
	{ VariantParameter::Value::type_curve, "curve" },
	{ VariantParameter::Value::type_gradient, "gradient" },
	{ VariantParameter::Value::type_resource_image, "resource_image" }
})

void to_json(nlohmann::ordered_json& j, const VariantParameter::Value& value);
void to_json(nlohmann::ordered_json& j, const VariantParameter& parameter);
void from_json(const nlohmann::ordered_json& j, VariantParameter::Value& value);
void from_json(const nlohmann::ordered_json& j, VariantParameter& parameter);
}