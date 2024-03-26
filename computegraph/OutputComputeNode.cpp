#include "OutputComputeNode.h"

namespace pixelpart {
std::string OutputComputeNode::typeName = "compute_node_output";

OutputComputeNode::OutputComputeNode() : ComputeNodeBase(typeName,
	"compute_category_output",
	{ "compute_slot_value" },
	{ },
	{
		Signature{ { VariantValue::type_bool }, { } },
		Signature{ { VariantValue::type_int }, { } },
		Signature{ { VariantValue::type_float }, { } },
		Signature{ { VariantValue::type_float2 }, { } },
		Signature{ { VariantValue::type_float3 }, { } },
		Signature{ { VariantValue::type_float4 }, { } },
	},
	{ VariantValue() },
	{ }) {

}

std::vector<VariantValue> OutputComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ in[0] };
}
}