#pragma once

#include "Curve.h"

namespace pixelpart {
struct VariantParameter {
	struct Value {
		enum Type : uint32_t {
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
		} type;

		struct Data {
			int_t integer = 0;
			float_t numbers[256] = { 0.0 };
			std::string string;
		} data;

		static Value IntValue(int_t v);
		static Value FloatValue(float_t v);
		static Value Float2Value(const vec2_t& v);
		static Value Float3Value(const vec3_t& v);
		static Value Float4Value(const vec4_t& v);
		static Value BoolValue(bool v);
		static Value EnumValue(int_t v);
		static Value ColorValue(const vec4_t& v);
		static Value CurveValue(const Curve<float_t>& v);
		static Value GradientValue(const Curve<vec3_t>& v);
		static Value GradientValue(const Curve<vec4_t>& v);
		static Value ImageResourceValue(const std::string& v);
		static Value ImageResourceValue(const std::string& id, const std::string& path);

		Value();

		int_t getInt() const;
		float_t getFloat() const;
		vec2_t getFloat2() const;
		vec3_t getFloat3() const;
		vec4_t getFloat4() const;
		bool getBool() const;
		int_t getEnum() const;
		vec4_t getColor() const;
		Curve<float_t> getCurve() const;
		Curve<vec3_t> getGradient() const;
		Curve<vec4_t> getGradient4() const;
		std::string getResourceString() const;
		std::string getResourceId() const;
		std::string getResourcePath() const;
	};

	static VariantParameter createIntParameter(const std::string& name, int_t def, int_t min, int_t max);
	static VariantParameter createFloatParameter(const std::string& name, float_t def, float_t min, float_t max);
	static VariantParameter createFloat2Parameter(const std::string& name, const vec2_t& def, const vec2_t& min, const vec2_t& max);
	static VariantParameter createFloat3Parameter(const std::string& name, const vec3_t& def, const vec3_t& min, const vec3_t& max);
	static VariantParameter createFloat4Parameter(const std::string& name, const vec4_t& def, const vec4_t& min, const vec4_t& max);
	static VariantParameter createBoolParameter(const std::string& name, bool def);
	static VariantParameter createEnumParameter(const std::string& name, int_t def, const std::vector<std::string>& options);
	static VariantParameter createColorParameter(const std::string& name, const vec4_t& def);
	static VariantParameter createCurveParameter(const std::string& name, const Curve<float_t>& def);
	static VariantParameter createGradientParameter(const std::string& name, const Curve<vec3_t>& def);
	static VariantParameter createGradientParameter(const std::string& name, const Curve<vec4_t>& def);
	static VariantParameter createImageResourceParameter(const std::string& name);

	std::string name;
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