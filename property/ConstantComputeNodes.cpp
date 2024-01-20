#include "ConstantComputeNodes.h"

namespace pixelpart {
std::string BooleanConstantComputeNode::typeName = "compute_node_boolean";
std::string IntegerConstantComputeNode::typeName = "compute_node_integer";
std::string NumberConstantComputeNode::typeName = "compute_node_number";
std::string Vector2ConstantComputeNode::typeName = "compute_node_vector2";
std::string Vector3ConstantComputeNode::typeName = "compute_node_vector3";
std::string Vector4ConstantComputeNode::typeName = "compute_node_vector4";
std::string ColorConstantComputeNode::typeName = "compute_node_color";

BooleanConstantComputeNode::BooleanConstantComputeNode() : ComputeNodeBase(typeName,
	"compute_category_constant",
	{ },
	{ "compute_slot_value" },
	{ Signature{ { }, { VariantValue::type_bool } } },
	{ },
	{ VariantParameter::createBoolParameter("compute_param_value", false) }) {

}
std::vector<VariantValue> BooleanConstantComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Bool(parameterValues[0].getBool()) };
}

IntegerConstantComputeNode::IntegerConstantComputeNode() : ComputeNodeBase(typeName,
	"compute_category_constant",
	{ },
	{ "compute_slot_value" },
	{ Signature{ { }, { VariantValue::type_int } } },
	{ },
	{ VariantParameter::createIntParameter("compute_param_value", 0, INT_MIN, INT_MAX) }) {

}
std::vector<VariantValue> IntegerConstantComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Int(parameterValues[0].getInt()) };
}

NumberConstantComputeNode::NumberConstantComputeNode() : ComputeNodeBase(typeName,
	"compute_category_constant",
	{ },
	{ "compute_slot_value" },
	{ Signature{ { }, { VariantValue::type_float } } },
	{ },
	{ VariantParameter::createFloatParameter("compute_param_value", 0.0, -FLT_MAX, +FLT_MAX) }) {

}
std::vector<VariantValue> NumberConstantComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Float(parameterValues[0].getFloat()) };
}

Vector2ConstantComputeNode::Vector2ConstantComputeNode() : ComputeNodeBase(typeName,
	"compute_category_constant",
	{ },
	{ "compute_slot_value" },
	{ Signature{ { }, { VariantValue::type_float2 } } },
	{ },
	{ VariantParameter::createFloat2Parameter("compute_param_value", vec2d(0.0), vec2d(-FLT_MAX), vec2d(+FLT_MAX)) }) {

}
std::vector<VariantValue> Vector2ConstantComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Float2(parameterValues[0].getFloat2()) };
}

Vector3ConstantComputeNode::Vector3ConstantComputeNode() : ComputeNodeBase(typeName,
	"compute_category_constant",
	{ },
	{ "compute_slot_value" },
	{ Signature{ { }, { VariantValue::type_float3 } } },
	{ },
	{ VariantParameter::createFloat3Parameter("compute_param_value", vec3d(0.0), vec3d(-FLT_MAX), vec3d(+FLT_MAX)) }) {

}
std::vector<VariantValue> Vector3ConstantComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Float3(parameterValues[0].getFloat3()) };
}

Vector4ConstantComputeNode::Vector4ConstantComputeNode() : ComputeNodeBase(typeName,
	"compute_category_constant",
	{ },
	{ "compute_slot_value" },
	{ Signature{ { }, { VariantValue::type_float4 } } },
	{ },
	{ VariantParameter::createFloat4Parameter("compute_param_value", vec4d(0.0), vec4d(-FLT_MAX), vec4d(+FLT_MAX)) }) {

}
std::vector<VariantValue> Vector4ConstantComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Float4(parameterValues[0].getFloat4()) };
}

ColorConstantComputeNode::ColorConstantComputeNode() : ComputeNodeBase(typeName,
	"compute_category_constant",
	{ },
	{ "compute_slot_value" },
	{ Signature{ { }, { VariantValue::type_float4 } } },
	{ },
	{ VariantParameter::createColorParameter("compute_param_value", vec4d(1.0)) }) {

}
std::vector<VariantValue> ColorConstantComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Float4(parameterValues[0].getFloat4()) };
}
}