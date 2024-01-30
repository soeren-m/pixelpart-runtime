#include "ComparisonComputeNodes.h"

namespace pixelpart {
std::string EqualComputeNode::typeName = "compute_node_equal";
std::string NotEqualComputeNode::typeName = "compute_node_notequal";
std::string ApproxEqualComputeNode::typeName = "compute_node_approxequal";
std::string LessComputeNode::typeName = "compute_node_less";
std::string LessOrEqualComputeNode::typeName = "compute_node_lessorequal";
std::string GreaterComputeNode::typeName = "compute_node_greater";
std::string GreaterOrEqualComputeNode::typeName = "compute_node_greaterorequal";

EqualComputeNode::EqualComputeNode() : ComputeNodeBase(typeName,
	"compute_category_comparison",
	{ "compute_slot_a", "compute_slot_b" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_bool, VariantValue::type_bool }, { VariantValue::type_bool } },
		Signature{ { VariantValue::type_int, VariantValue::type_int }, { VariantValue::type_bool } },
		Signature{ { VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_bool } },
		Signature{ { VariantValue::type_float2, VariantValue::type_float2 }, { VariantValue::type_bool } },
		Signature{ { VariantValue::type_float3, VariantValue::type_float3 }, { VariantValue::type_bool } },
		Signature{ { VariantValue::type_float4, VariantValue::type_float4 }, { VariantValue::type_bool } }
	},
	{ VariantValue(), VariantValue() },
	{ }) {

}
std::vector<VariantValue> EqualComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Bool(in[0] == in[1]) };
}

NotEqualComputeNode::NotEqualComputeNode() : ComputeNodeBase(typeName,
	"compute_category_comparison",
	{ "compute_slot_a", "compute_slot_b" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_bool, VariantValue::type_bool }, { VariantValue::type_bool } },
		Signature{ { VariantValue::type_int, VariantValue::type_int }, { VariantValue::type_bool } },
		Signature{ { VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_bool } },
		Signature{ { VariantValue::type_float2, VariantValue::type_float2 }, { VariantValue::type_bool } },
		Signature{ { VariantValue::type_float3, VariantValue::type_float3 }, { VariantValue::type_bool } },
		Signature{ { VariantValue::type_float4, VariantValue::type_float4 }, { VariantValue::type_bool } }
	},
	{ VariantValue(), VariantValue() },
	{ }) {

}
std::vector<VariantValue> NotEqualComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Bool(in[0] != in[1]) };
}

ApproxEqualComputeNode::ApproxEqualComputeNode() : ComputeNodeBase(typeName,
	"compute_category_comparison",
	{ "compute_slot_a", "compute_slot_b" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_int, VariantValue::type_int }, { VariantValue::type_bool } },
		Signature{ { VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_bool } }
	},
	{ VariantValue(), VariantValue() },
	{ VariantParameter::createFloatParameter("compute_param_epsilon", 0.001, 0.0, FLT_MAX) }) {

}
std::vector<VariantValue> ApproxEqualComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{
		VariantValue::Bool(glm::abs(in[0].toFloat() - in[1].toFloat()) <= parameterValues[0].getFloat())
	};
}

LessComputeNode::LessComputeNode() : ComputeNodeBase(typeName,
	"compute_category_comparison",
	{ "compute_slot_a", "compute_slot_b" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_int, VariantValue::type_int }, { VariantValue::type_bool } },
		Signature{ { VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_bool } }
	},
	{ VariantValue(), VariantValue() },
	{ }) {

}
std::vector<VariantValue> LessComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Bool(in[0] < in[1]) };
}

LessOrEqualComputeNode::LessOrEqualComputeNode() : ComputeNodeBase(typeName,
	"compute_category_comparison",
	{ "compute_slot_a", "compute_slot_b" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_int, VariantValue::type_int }, { VariantValue::type_bool } },
		Signature{ { VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_bool } }
	},
	{ VariantValue(), VariantValue() },
	{ }) {

}
std::vector<VariantValue> LessOrEqualComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Bool(in[0] <= in[1]) };
}

GreaterComputeNode::GreaterComputeNode() : ComputeNodeBase(typeName,
	"compute_category_comparison",
	{ "compute_slot_a", "compute_slot_b" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_int, VariantValue::type_int }, { VariantValue::type_bool } },
		Signature{ { VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_bool } }
	},
	{ VariantValue(), VariantValue() },
	{ }) {

}
std::vector<VariantValue> GreaterComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Bool(in[0] > in[1]) };
}

GreaterOrEqualComputeNode::GreaterOrEqualComputeNode() : ComputeNodeBase(typeName,
	"compute_category_comparison",
	{ "compute_slot_a", "compute_slot_b" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_int, VariantValue::type_int }, { VariantValue::type_bool } },
		Signature{ { VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_bool } }
	},
	{ VariantValue(), VariantValue() },
	{ }) {

}
std::vector<VariantValue> GreaterOrEqualComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Bool(in[0] >= in[1]) };
}
}