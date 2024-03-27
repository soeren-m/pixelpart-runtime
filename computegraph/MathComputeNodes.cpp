#include "MathComputeNodes.h"

namespace pixelpart {
std::string AddComputeNode::typeName = "compute_node_add";
std::string SubtractComputeNode::typeName = "compute_node_subtract";
std::string MultiplyComputeNode::typeName = "compute_node_multiply";
std::string DivideComputeNode::typeName = "compute_node_divide";
std::string ModComputeNode::typeName = "compute_node_mod";
std::string PowComputeNode::typeName = "compute_node_pow";
std::string ExpComputeNode::typeName = "compute_node_exp";
std::string LogComputeNode::typeName = "compute_node_log";
std::string SignComputeNode::typeName = "compute_node_sign";
std::string AbsComputeNode::typeName = "compute_node_abs";
std::string MinComputeNode::typeName = "compute_node_min";
std::string MaxComputeNode::typeName = "compute_node_max";
std::string ClampComputeNode::typeName = "compute_node_clamp";
std::string LerpComputeNode::typeName = "compute_node_lerp";
std::string FloorComputeNode::typeName = "compute_node_floor";
std::string CeilComputeNode::typeName = "compute_node_ceil";
std::string RoundComputeNode::typeName = "compute_node_round";
std::string SqrtComputeNode::typeName = "compute_node_sqrt";
std::string SinComputeNode::typeName = "compute_node_sin";
std::string CosComputeNode::typeName = "compute_node_cos";
std::string AsinComputeNode::typeName = "compute_node_asin";
std::string AcosComputeNode::typeName = "compute_node_acos";
std::string DotComputeNode::typeName = "compute_node_dot";
std::string CrossComputeNode::typeName = "compute_node_cross";
std::string NormalizeComputeNode::typeName = "compute_node_normalize";
std::string VectorLengthComputeNode::typeName = "compute_node_vectorlength";
std::string StepComputeNode::typeName = "compute_node_step";
std::string SmoothstepComputeNode::typeName = "compute_node_smoothstep";

AddComputeNode::AddComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_a", "compute_slot_b" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_int, VariantValue::type_int }, { VariantValue::type_int } },
		Signature{ { VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2, VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3, VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4, VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue::Int(0), VariantValue::Int(0) },
	{ }) {

}
std::vector<VariantValue> AddComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ in[0] + in[1] };
}

SubtractComputeNode::SubtractComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_a", "compute_slot_b" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_int, VariantValue::type_int }, { VariantValue::type_int } },
		Signature{ { VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2, VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3, VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4, VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue::Int(0), VariantValue::Int(0) },
	{ }) {

}
std::vector<VariantValue> SubtractComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ in[0] - in[1] };
}

MultiplyComputeNode::MultiplyComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_a", "compute_slot_b" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_int, VariantValue::type_int }, { VariantValue::type_int } },
		Signature{ { VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2, VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3, VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4, VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue::Int(1), VariantValue::Int(1) },
	{ }) {

}
std::vector<VariantValue> MultiplyComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ in[0] * in[1] };
}

DivideComputeNode::DivideComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_a", "compute_slot_b" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_int, VariantValue::type_int }, { VariantValue::type_int } },
		Signature{ { VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2, VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3, VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4, VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue::Int(1), VariantValue::Int(1) },
	{ }) {

}
std::vector<VariantValue> DivideComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ in[0] / in[1] };
}

ModComputeNode::ModComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_a", "compute_slot_b" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_int, VariantValue::type_int }, { VariantValue::type_int } },
		Signature{ { VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2, VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3, VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4, VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue(), VariantValue::Int(1) },
	{ }) {

}
std::vector<VariantValue> ModComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_int:
			result = VariantValue::Int(in[0].getInt() % in[1].toInt());
			break;
		case VariantValue::type_float:
			result = VariantValue::Float(glm::mod(in[0].getFloat(), in[1].toFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(glm::mod(in[0].getFloat2(), in[1].toFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(glm::mod(in[0].getFloat3(), in[1].toFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(glm::mod(in[0].getFloat4(), in[1].toFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}

PowComputeNode::PowComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_a", "compute_slot_b" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_int, VariantValue::type_int }, { VariantValue::type_int } },
		Signature{ { VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2, VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3, VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4, VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue(), VariantValue::Float(1.0) },
	{ }) {

}
std::vector<VariantValue> PowComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_int:
			result = VariantValue::Int(glm::pow(in[0].getInt(), in[1].toInt()));
			break;
		case VariantValue::type_float:
			result = VariantValue::Float(glm::pow(in[0].getFloat(), in[1].toFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(glm::pow(in[0].getFloat2(), in[1].toFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(glm::pow(in[0].getFloat3(), in[1].toFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(glm::pow(in[0].getFloat4(), in[1].toFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}

ExpComputeNode::ExpComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_value" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue() },
	{ }) {

}
std::vector<VariantValue> ExpComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_float:
			result = VariantValue::Float(glm::exp(in[0].getFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(glm::exp(in[0].getFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(glm::exp(in[0].getFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(glm::exp(in[0].getFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}

LogComputeNode::LogComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_value" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue() },
	{ }) {

}
std::vector<VariantValue> LogComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_float:
			result = VariantValue::Float(glm::log(in[0].getFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(glm::log(in[0].getFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(glm::log(in[0].getFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(glm::log(in[0].getFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}

SignComputeNode::SignComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_value" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_int }, { VariantValue::type_int } },
		Signature{ { VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue() },
	{ }) {

}
std::vector<VariantValue> SignComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_int:
			result = VariantValue::Int(glm::sign(in[0].getInt()));
			break;
		case VariantValue::type_float:
			result = VariantValue::Float(glm::sign(in[0].getFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(glm::sign(in[0].getFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(glm::sign(in[0].getFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(glm::sign(in[0].getFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}

AbsComputeNode::AbsComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_value" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_int }, { VariantValue::type_int } },
		Signature{ { VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue() },
	{ }) {

}
std::vector<VariantValue> AbsComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_int:
			result = VariantValue::Int(glm::abs(in[0].getInt()));
			break;
		case VariantValue::type_float:
			result = VariantValue::Float(glm::abs(in[0].getFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(glm::abs(in[0].getFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(glm::abs(in[0].getFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(glm::abs(in[0].getFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}

MinComputeNode::MinComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_a", "compute_slot_b" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_int, VariantValue::type_int }, { VariantValue::type_int } },
		Signature{ { VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2, VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3, VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4, VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue(), VariantValue() },
	{ }) {

}
std::vector<VariantValue> MinComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_int:
			result = VariantValue::Int(glm::min(in[0].getInt(), in[1].toInt()));
			break;
		case VariantValue::type_float:
			result = VariantValue::Float(glm::min(in[0].getFloat(), in[1].toFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(glm::min(in[0].getFloat2(), in[1].toFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(glm::min(in[0].getFloat3(), in[1].toFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(glm::min(in[0].getFloat4(), in[1].toFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}

MaxComputeNode::MaxComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_a", "compute_slot_b" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_int, VariantValue::type_int }, { VariantValue::type_int } },
		Signature{ { VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2, VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3, VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4, VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue(), VariantValue() },
	{ }) {

}
std::vector<VariantValue> MaxComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_int:
			result = VariantValue::Int(glm::max(in[0].getInt(), in[1].toInt()));
			break;
		case VariantValue::type_float:
			result = VariantValue::Float(glm::max(in[0].getFloat(), in[1].toFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(glm::max(in[0].getFloat2(), in[1].toFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(glm::max(in[0].getFloat3(), in[1].toFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(glm::max(in[0].getFloat4(), in[1].toFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}

ClampComputeNode::ClampComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_value", "compute_slot_min", "compute_slot_max" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_int, VariantValue::type_int, VariantValue::type_int }, { VariantValue::type_int } },
		Signature{ { VariantValue::type_float, VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2, VariantValue::type_float2, VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3, VariantValue::type_float3, VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4, VariantValue::type_float4, VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue(), VariantValue::Float(0.0), VariantValue::Float(1.0) },
	{ }) {

}
std::vector<VariantValue> ClampComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_int:
			result = VariantValue::Int(glm::clamp(in[0].getInt(), in[1].toInt(), in[2].toInt()));
			break;
		case VariantValue::type_float:
			result = VariantValue::Float(glm::clamp(in[0].getFloat(), in[1].toFloat(), in[2].toFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(glm::clamp(in[0].getFloat2(), in[1].toFloat2(), in[2].toFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(glm::clamp(in[0].getFloat3(), in[1].toFloat3(), in[2].toFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(glm::clamp(in[0].getFloat4(), in[1].toFloat4(), in[2].toFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}

LerpComputeNode::LerpComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_a", "compute_slot_b", "compute_slot_factor" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_float, VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2, VariantValue::type_float2, VariantValue::type_float }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3, VariantValue::type_float3, VariantValue::type_float }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4, VariantValue::type_float4, VariantValue::type_float }, { VariantValue::type_float4 } }
	},
	{ VariantValue(), VariantValue(), VariantValue::Float(0.5) },
	{ }) {

}
std::vector<VariantValue> LerpComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_float:
			result = VariantValue::Float(glm::mix(in[0].getFloat(), in[1].toFloat(), in[2].toFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(glm::mix(in[0].getFloat2(), in[1].toFloat2(), in[2].toFloat()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(glm::mix(in[0].getFloat3(), in[1].toFloat3(), in[2].toFloat()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(glm::mix(in[0].getFloat4(), in[1].toFloat4(), in[2].toFloat()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}

FloorComputeNode::FloorComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_value" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue() },
	{ }) {

}
std::vector<VariantValue> FloorComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_float:
			result = VariantValue::Float(glm::floor(in[0].getFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(glm::floor(in[0].getFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(glm::floor(in[0].getFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(glm::floor(in[0].getFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}

CeilComputeNode::CeilComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_value" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue() },
	{ }) {

}
std::vector<VariantValue> CeilComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_float:
			result = VariantValue::Float(glm::ceil(in[0].getFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(glm::ceil(in[0].getFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(glm::ceil(in[0].getFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(glm::ceil(in[0].getFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}

RoundComputeNode::RoundComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_value" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue() },
	{ }) {

}
std::vector<VariantValue> RoundComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_float:
			result = VariantValue::Float(glm::round(in[0].getFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(glm::round(in[0].getFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(glm::round(in[0].getFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(glm::round(in[0].getFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}

SqrtComputeNode::SqrtComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_value" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue() },
	{ }) {

}
std::vector<VariantValue> SqrtComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_float:
			result = VariantValue::Float(glm::sqrt(in[0].getFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(glm::sqrt(in[0].getFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(glm::sqrt(in[0].getFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(glm::sqrt(in[0].getFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}

SinComputeNode::SinComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_value" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue() },
	{ }) {

}
std::vector<VariantValue> SinComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_float:
			result = VariantValue::Float(glm::sin(in[0].getFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(glm::sin(in[0].getFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(glm::sin(in[0].getFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(glm::sin(in[0].getFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}

CosComputeNode::CosComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_value" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue() },
	{ }) {

}
std::vector<VariantValue> CosComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_float:
			result = VariantValue::Float(glm::cos(in[0].getFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(glm::cos(in[0].getFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(glm::cos(in[0].getFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(glm::cos(in[0].getFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}

AsinComputeNode::AsinComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_value" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue() },
	{ }) {

}
std::vector<VariantValue> AsinComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_float:
			result = VariantValue::Float(glm::asin(in[0].getFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(glm::asin(in[0].getFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(glm::asin(in[0].getFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(glm::asin(in[0].getFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}

AcosComputeNode::AcosComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_value" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue() },
	{ }) {

}
std::vector<VariantValue> AcosComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_float:
			result = VariantValue::Float(glm::acos(in[0].getFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(glm::acos(in[0].getFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(glm::acos(in[0].getFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(glm::acos(in[0].getFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}

DotComputeNode::DotComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_a", "compute_slot_b" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_float2, VariantValue::type_float2 }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float3, VariantValue::type_float3 }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float4, VariantValue::type_float4 }, { VariantValue::type_float } }
	},
	{ VariantValue(), VariantValue() },
	{ }) {

}
std::vector<VariantValue> DotComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_float2:
			result = VariantValue::Float(glm::dot(in[0].getFloat2(), in[1].toFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float(glm::dot(in[0].getFloat3(), in[1].toFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float(glm::dot(in[0].getFloat4(), in[1].toFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}

CrossComputeNode::CrossComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_a", "compute_slot_b" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_float3, VariantValue::type_float3 }, { VariantValue::type_float3 } }
	},
	{ VariantValue(), VariantValue() },
	{ }) {

}
std::vector<VariantValue> CrossComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{ VariantValue::Float3(glm::cross(in[0].toFloat3(), in[1].toFloat3())) };
}

NormalizeComputeNode::NormalizeComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_vector" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue() },
	{ }) {

}
std::vector<VariantValue> NormalizeComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_float2:
			result = VariantValue::Float2(glm::normalize(in[0].getFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(glm::normalize(in[0].getFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(glm::normalize(in[0].getFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}

VectorLengthComputeNode::VectorLengthComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_vector" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_float2 }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float3 }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float4 }, { VariantValue::type_float } }
	},
	{ VariantValue() },
	{ }) {

}
std::vector<VariantValue> VectorLengthComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_float2:
			result = VariantValue::Float(glm::length(in[0].getFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float(glm::length(in[0].getFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float(glm::length(in[0].getFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}

StepComputeNode::StepComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_value", "compute_slot_edge" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2, VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3, VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4, VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue(), VariantValue::Float(0.0) },
	{ }) {

}
std::vector<VariantValue> StepComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_float:
			result = VariantValue::Float(glm::step(in[1].toFloat(), in[0].getFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(glm::step(in[1].toFloat2(), in[0].getFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(glm::step(in[1].toFloat3(), in[0].getFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(glm::step(in[1].toFloat4(), in[0].getFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}

SmoothstepComputeNode::SmoothstepComputeNode() : ComputeNodeBase(typeName,
	"compute_category_math",
	{ "compute_slot_value", "compute_slot_edge1", "compute_slot_edge2" },
	{ "compute_slot_result" },
	{
		Signature{ { VariantValue::type_float, VariantValue::type_float, VariantValue::type_float }, { VariantValue::type_float } },
		Signature{ { VariantValue::type_float2, VariantValue::type_float2, VariantValue::type_float2 }, { VariantValue::type_float2 } },
		Signature{ { VariantValue::type_float3, VariantValue::type_float3, VariantValue::type_float3 }, { VariantValue::type_float3 } },
		Signature{ { VariantValue::type_float4, VariantValue::type_float4, VariantValue::type_float4 }, { VariantValue::type_float4 } }
	},
	{ VariantValue(), VariantValue::Float(0.0), VariantValue::Float(1.0) },
	{ }) {

}
std::vector<VariantValue> SmoothstepComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result;
	switch(in[0].type) {
		case VariantValue::type_float:
			result = VariantValue::Float(glm::smoothstep(in[1].toFloat(), in[2].toFloat(), in[0].getFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(glm::smoothstep(in[1].toFloat2(), in[2].toFloat2(), in[0].getFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(glm::smoothstep(in[1].toFloat3(), in[2].toFloat3(), in[0].getFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(glm::smoothstep(in[1].toFloat4(), in[2].toFloat4(), in[0].getFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}
}