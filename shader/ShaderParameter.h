#pragma once

#include "../common/Curve.h"

namespace pixelpart {
struct ShaderParameter {
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
			type_path,
			type_gradient,
			type_resource_image
		} type;

		struct Data {
			int32_t integer = 0;
			floatd numbers[256] = { 0.0 };
			std::string string;
		} data;

		static Value IntValue(int32_t v);
		static Value FloatValue(floatd v);
		static Value Float2Value(const vec2d& v);
		static Value Float3Value(const vec3d& v);
		static Value Float4Value(const vec4d& v);
		static Value BoolValue(bool v);
		static Value EnumValue(int32_t v);
		static Value ColorValue(const vec4d& v);
		static Value CurveValue(const Curve<floatd>& v);
		static Value PathValue(const Curve<vec2d>& v);
		static Value GradientValue(const Curve<vec3d>& v);
		static Value GradientValue(const Curve<vec4d>& v);
		static Value ImageResourceValue(const std::string& v);
		static Value ImageResourceValue(const std::string& id, const std::string& path);

		Value();

		int32_t getInt() const;
		floatd getFloat() const;
		vec2d getFloat2() const;
		vec3d getFloat3() const;
		vec4d getFloat4() const;
		bool getBool() const;
		int32_t getEnum() const;
		vec4d getColor() const;
		Curve<floatd> getCurve() const;
		Curve<vec2d> getPath() const;
		Curve<vec3d> getGradient() const;
		Curve<vec4d> getGradient4() const;
		std::string getResourceString() const;
		std::string getResourceId() const;
		std::string getResourcePath() const;
	};

	static ShaderParameter createIntParameter(const std::string& name, int32_t def, int32_t min, int32_t max);
	static ShaderParameter createFloatParameter(const std::string& name, floatd def, floatd min, floatd max);
	static ShaderParameter createFloat2Parameter(const std::string& name, const vec2d& def, const vec2d& min, const vec2d& max);
	static ShaderParameter createFloat3Parameter(const std::string& name, const vec3d& def, const vec3d& min, const vec3d& max);
	static ShaderParameter createFloat4Parameter(const std::string& name, const vec4d& def, const vec4d& min, const vec4d& max);
	static ShaderParameter createBoolParameter(const std::string& name, bool def);
	static ShaderParameter createEnumParameter(const std::string& name, int32_t def, const std::vector<std::string>& options);
	static ShaderParameter createColorParameter(const std::string& name, const vec4d& def);
	static ShaderParameter createCurveParameter(const std::string& name, const Curve<floatd>& def);
	static ShaderParameter createPathParameter(const std::string& name, const Curve<vec2d>& def);
	static ShaderParameter createGradientParameter(const std::string& name, const Curve<vec3d>& def);
	static ShaderParameter createImageResourceParameter(const std::string& name);

	std::string name;
	Value defaultValue;
	Value minValue;
	Value maxValue;
	std::vector<std::string> valueNames;
};

NLOHMANN_JSON_SERIALIZE_ENUM(ShaderParameter::Value::Type, {
	{ ShaderParameter::Value::type_int, "int" },
	{ ShaderParameter::Value::type_float, "float" },
	{ ShaderParameter::Value::type_float2, "float2" },
	{ ShaderParameter::Value::type_float3, "float3" },
	{ ShaderParameter::Value::type_float4, "float4" },
	{ ShaderParameter::Value::type_bool, "bool" },
	{ ShaderParameter::Value::type_enum, "enum" },
	{ ShaderParameter::Value::type_color, "color" },
	{ ShaderParameter::Value::type_curve, "curve" },
	{ ShaderParameter::Value::type_path, "path" },
	{ ShaderParameter::Value::type_gradient, "gradient" },
	{ ShaderParameter::Value::type_resource_image, "resource_image" }
})

void to_json(nlohmann::ordered_json& j, const ShaderParameter::Value& value);
void to_json(nlohmann::ordered_json& j, const ShaderParameter& parameter);
void from_json(const nlohmann::ordered_json& j, ShaderParameter::Value& value);
void from_json(const nlohmann::ordered_json& j, ShaderParameter& parameter);
}