#include "LogicComputeNodes.h"

namespace pixelpart {
std::string NegateComputeNode::typeName = "compute_node_negate";
std::string AndComputeNode::typeName = "compute_node_and";
std::string OrComputeNode::typeName = "compute_node_or";
std::string XorComputeNode::typeName = "compute_node_xor";
std::string BranchComputeNode::typeName = "compute_node_branch";

NegateComputeNode::NegateComputeNode() : ComputeNodeBase(typeName,
	"compute_category_logic",
	{ "compute_slot_value" },
	{ "compute_slot_result" },
	{ Signature{ { VariantValue::type_bool }, { VariantValue::type_bool } } },
	{ VariantValue() },
	{ }) {

}
std::vector<VariantValue> NegateComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Bool(!in[0].toBool()) };
}

AndComputeNode::AndComputeNode() : ComputeNodeBase(typeName,
	"compute_category_logic",
	{ "compute_slot_a", "compute_slot_b" },
	{ "compute_slot_result" },
	{ Signature{ { VariantValue::type_bool, VariantValue::type_bool }, { VariantValue::type_bool } } },
	{ VariantValue(), VariantValue() },
	{ }) {

}
std::vector<VariantValue> AndComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Bool(in[0].toBool() && in[1].toBool()) };
}

OrComputeNode::OrComputeNode() : ComputeNodeBase(typeName,
	"compute_category_logic",
	{ "compute_slot_a", "compute_slot_b" },
	{ "compute_slot_result" },
	{ Signature{ { VariantValue::type_bool, VariantValue::type_bool }, { VariantValue::type_bool } } },
	{ VariantValue(), VariantValue() },
	{ }) {

}
std::vector<VariantValue> OrComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Bool(in[0].toBool() || in[1].toBool()) };
}

XorComputeNode::XorComputeNode() : ComputeNodeBase(typeName,
	"compute_category_logic",
	{ "compute_slot_a", "compute_slot_b" },
	{ "compute_slot_result" },
	{ Signature{ { VariantValue::type_bool, VariantValue::type_bool }, { VariantValue::type_bool } } },
	{ VariantValue(), VariantValue() },
	{ }) {

}
std::vector<VariantValue> XorComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Bool(in[0].toBool() != in[1].toBool()) };
}

BranchComputeNode::BranchComputeNode() : ComputeNodeBase(typeName,
	"compute_category_logic",
	{ "compute_slot_predicate", "compute_slot_true", "compute_slot_false" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_bool, VariantValue::type_bool, VariantValue::type_bool }, { VariantValue::type_bool } },
		Signature{ { VariantValue::type_bool, VariantValue::type_int, VariantValue::type_int }, { VariantValue::type_int } },
		Signature{ { VariantValue::type_bool, VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_bool, VariantValue::type_float2, VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_bool, VariantValue::type_float3, VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_bool, VariantValue::type_float4, VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue(), VariantValue(), VariantValue() },
	{ }) {

}
std::vector<VariantValue> BranchComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ in[0].toBool() ? in[1] : in[2] };
}
}
