#include "UtilComputeNodes.h"

namespace pixelpart {
std::string CurveComputeNode::typeName = "compute_node_curve";
std::string Split2ComputeNode::typeName = "compute_node_split2";
std::string Split3ComputeNode::typeName = "compute_node_split3";
std::string Split4ComputeNode::typeName = "compute_node_split4";
std::string Merge2ComputeNode::typeName = "compute_node_merge2";
std::string Merge3ComputeNode::typeName = "compute_node_merge3";
std::string Merge4ComputeNode::typeName = "compute_node_merge4";

CurveComputeNode::CurveComputeNode() : ComputeNodeBase(typeName,
	"compute_category_util",
	{ "compute_slot_value" },
	{ "compute_slot_result" },
	{ Signature{ { VariantValue::type_float }, { VariantValue::type_float } } },
	{ VariantValue::Float(0.0) },
	{ VariantParameter::createCurveParameter("compute_param_curve", Curve<floatd>(0.5, CurveInterpolation::linear)) }) {

}
std::vector<VariantValue> CurveComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Float(parameterValues[0].getCurve().get(in[0].toFloat())) };
}

Split2ComputeNode::Split2ComputeNode() : ComputeNodeBase(typeName,
	"compute_category_util",
	{ "compute_slot_vector" },
	{ "compute_slot_x", "compute_slot_y" },
	{ Signature{ { VariantValue::type_float2 }, { VariantValue::type_float, VariantValue::type_float } } },
	{ VariantValue() },
	{ }) {

}
std::vector<VariantValue> Split2ComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{
		VariantValue::Float(in[0].toFloat2().x),
		VariantValue::Float(in[0].toFloat2().y)
	};
}

Split3ComputeNode::Split3ComputeNode() : ComputeNodeBase(typeName,
	"compute_category_util",
	{ "compute_slot_vector" },
	{ "compute_slot_x", "compute_slot_y", "compute_slot_z" },
	{ Signature{ { VariantValue::type_float3 }, { VariantValue::type_float, VariantValue::type_float, VariantValue::type_float } } },
	{ VariantValue() },
	{ }) {

}
std::vector<VariantValue> Split3ComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{
		VariantValue::Float(in[0].toFloat3().x),
		VariantValue::Float(in[0].toFloat3().y),
		VariantValue::Float(in[0].toFloat3().z)
	};
}

Split4ComputeNode::Split4ComputeNode() : ComputeNodeBase(typeName,
	"compute_category_util",
	{ "compute_slot_vector" },
	{ "compute_slot_x", "compute_slot_y", "compute_slot_z", "compute_slot_w" },
	{ Signature{ { VariantValue::type_float4 }, { VariantValue::type_float, VariantValue::type_float, VariantValue::type_float, VariantValue::type_float } } },
	{ VariantValue() },
	{ }) {

}
std::vector<VariantValue> Split4ComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{
		VariantValue::Float(in[0].toFloat4().x),
		VariantValue::Float(in[0].toFloat4().y),
		VariantValue::Float(in[0].toFloat4().z),
		VariantValue::Float(in[0].toFloat4().w)
	};
}

Merge2ComputeNode::Merge2ComputeNode() : ComputeNodeBase(typeName,
	"compute_category_util",
	{ "compute_slot_x", "compute_slot_y" },
	{ "compute_slot_vector" },
	{ Signature{ { VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_float2 } } },
	{ VariantValue::Float(0.0), VariantValue::Float(0.0) },
	{ }) {

}
std::vector<VariantValue> Merge2ComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{
		VariantValue::Float2(vec2d(in[0].toFloat(), in[1].toFloat()))
	};
}

Merge3ComputeNode::Merge3ComputeNode() : ComputeNodeBase(typeName,
	"compute_category_util",
	{ "compute_slot_x", "compute_slot_y", "compute_slot_z" },
	{ "compute_slot_vector" },
	{ Signature{ { VariantValue::type_float, VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_float3 } } },
	{ VariantValue::Float(0.0), VariantValue::Float(0.0), VariantValue::Float(0.0) },
	{ }) {

}
std::vector<VariantValue> Merge3ComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{
		VariantValue::Float3(vec3d(in[0].toFloat(), in[1].toFloat(), in[2].toFloat()))
	};
}

Merge4ComputeNode::Merge4ComputeNode() : ComputeNodeBase(typeName,
	"compute_category_util",
	{ "compute_slot_x", "compute_slot_y", "compute_slot_z", "compute_slot_w" },
	{ "compute_slot_vector" },
	{ Signature{ { VariantValue::type_float, VariantValue::type_float, VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_float4 } } },
	{ VariantValue::Float(0.0), VariantValue::Float(0.0), VariantValue::Float(0.0), VariantValue::Float(0.0) },
	{ }) {

}
std::vector<VariantValue> Merge4ComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{
		VariantValue::Float4(vec4d(in[0].toFloat(), in[1].toFloat(), in[2].toFloat(), in[3].toFloat()))
	};
}
}