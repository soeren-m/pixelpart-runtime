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
	{ VariantParameter::BoolParameter("compute_param_value", false) }) {

}
std::vector<VariantValue> BooleanConstantComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Bool(parameterValue(0).valueBool()) };
}

IntegerConstantComputeNode::IntegerConstantComputeNode() : ComputeNodeBase(typeName,
	"compute_category_constant",
	{ },
	{ "compute_slot_value" },
	{ Signature{ { }, { VariantValue::type_int } } },
	{ },
	{ VariantParameter::IntParameter("compute_param_value", 0, INT_MIN, INT_MAX) }) {

}
std::vector<VariantValue> IntegerConstantComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Int(parameterValue(0).valueInt()) };
}

NumberConstantComputeNode::NumberConstantComputeNode() : ComputeNodeBase(typeName,
	"compute_category_constant",
	{ },
	{ "compute_slot_value" },
	{ Signature{ { }, { VariantValue::type_float } } },
	{ },
	{ VariantParameter::FloatParameter("compute_param_value", 0.0, -FLT_MAX, +FLT_MAX) }) {

}
std::vector<VariantValue> NumberConstantComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Float(parameterValue(0).valueFloat()) };
}

Vector2ConstantComputeNode::Vector2ConstantComputeNode() : ComputeNodeBase(typeName,
	"compute_category_constant",
	{ },
	{ "compute_slot_value" },
	{ Signature{ { }, { VariantValue::type_float2 } } },
	{ },
	{ VariantParameter::Float2Parameter("compute_param_value", float2_t(0.0), float2_t(-FLT_MAX), float2_t(+FLT_MAX)) }) {

}
std::vector<VariantValue> Vector2ConstantComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Float2(parameterValue(0).valueFloat2()) };
}

Vector3ConstantComputeNode::Vector3ConstantComputeNode() : ComputeNodeBase(typeName,
	"compute_category_constant",
	{ },
	{ "compute_slot_value" },
	{ Signature{ { }, { VariantValue::type_float3 } } },
	{ },
	{ VariantParameter::Float3Parameter("compute_param_value", float3_t(0.0), float3_t(-FLT_MAX), float3_t(+FLT_MAX)) }) {

}
std::vector<VariantValue> Vector3ConstantComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Float3(parameterValue(0).valueFloat3()) };
}

Vector4ConstantComputeNode::Vector4ConstantComputeNode() : ComputeNodeBase(typeName,
	"compute_category_constant",
	{ },
	{ "compute_slot_value" },
	{ Signature{ { }, { VariantValue::type_float4 } } },
	{ },
	{ VariantParameter::Float4Parameter("compute_param_value", float4_t(0.0), float4_t(-FLT_MAX), float4_t(+FLT_MAX)) }) {

}
std::vector<VariantValue> Vector4ConstantComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Float4(parameterValue(0).valueFloat4()) };
}

ColorConstantComputeNode::ColorConstantComputeNode() : ComputeNodeBase(typeName,
	"compute_category_constant",
	{ },
	{ "compute_slot_value" },
	{ Signature{ { }, { VariantValue::type_float4 } } },
	{ },
	{ VariantParameter::ColorParameter("compute_param_value", float4_t(1.0)) }) {

}
std::vector<VariantValue> ColorConstantComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Float4(parameterValue(0).valueFloat4()) };
}
}
