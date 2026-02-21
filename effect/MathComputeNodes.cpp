#include "MathComputeNodes.h"
#include "../math/Common.h"
#include "../math/Exponent.h"
#include "../math/Trigonometry.h"
#include "../math/Geometry.h"
#include "../math/Interpolation.h"

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
	switch(in[0].type()) {
		case VariantValue::type_int:
			result = VariantValue::Int(in[0].valueInt() % in[1].toInt());
			break;
		case VariantValue::type_float:
			result = VariantValue::Float(math::mod(in[0].valueFloat(), in[1].toFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(math::mod(in[0].valueFloat2(), in[1].toFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(math::mod(in[0].valueFloat3(), in[1].toFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(math::mod(in[0].valueFloat4(), in[1].toFloat4()));
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
	switch(in[0].type()) {
		case VariantValue::type_int:
			result = VariantValue::Int(math::pow(in[0].valueInt(), in[1].toInt()));
			break;
		case VariantValue::type_float:
			result = VariantValue::Float(math::pow(in[0].valueFloat(), in[1].toFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(math::pow(in[0].valueFloat2(), in[1].toFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(math::pow(in[0].valueFloat3(), in[1].toFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(math::pow(in[0].valueFloat4(), in[1].toFloat4()));
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
	switch(in[0].type()) {
		case VariantValue::type_float:
			result = VariantValue::Float(math::exp(in[0].valueFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(math::exp(in[0].valueFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(math::exp(in[0].valueFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(math::exp(in[0].valueFloat4()));
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
	switch(in[0].type()) {
		case VariantValue::type_float:
			result = VariantValue::Float(math::log(in[0].valueFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(math::log(in[0].valueFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(math::log(in[0].valueFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(math::log(in[0].valueFloat4()));
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
	switch(in[0].type()) {
		case VariantValue::type_int:
			result = VariantValue::Int(math::sign(in[0].valueInt()));
			break;
		case VariantValue::type_float:
			result = VariantValue::Float(math::sign(in[0].valueFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(math::sign(in[0].valueFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(math::sign(in[0].valueFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(math::sign(in[0].valueFloat4()));
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
	switch(in[0].type()) {
		case VariantValue::type_int:
			result = VariantValue::Int(math::abs(in[0].valueInt()));
			break;
		case VariantValue::type_float:
			result = VariantValue::Float(math::abs(in[0].valueFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(math::abs(in[0].valueFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(math::abs(in[0].valueFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(math::abs(in[0].valueFloat4()));
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
	switch(in[0].type()) {
		case VariantValue::type_int:
			result = VariantValue::Int(math::min(in[0].valueInt(), in[1].toInt()));
			break;
		case VariantValue::type_float:
			result = VariantValue::Float(math::min(in[0].valueFloat(), in[1].toFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(math::min(in[0].valueFloat2(), in[1].toFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(math::min(in[0].valueFloat3(), in[1].toFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(math::min(in[0].valueFloat4(), in[1].toFloat4()));
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
	switch(in[0].type()) {
		case VariantValue::type_int:
			result = VariantValue::Int(math::max(in[0].valueInt(), in[1].toInt()));
			break;
		case VariantValue::type_float:
			result = VariantValue::Float(math::max(in[0].valueFloat(), in[1].toFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(math::max(in[0].valueFloat2(), in[1].toFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(math::max(in[0].valueFloat3(), in[1].toFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(math::max(in[0].valueFloat4(), in[1].toFloat4()));
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
	switch(in[0].type()) {
		case VariantValue::type_int:
			result = VariantValue::Int(math::clamp(in[0].valueInt(), in[1].toInt(), in[2].toInt()));
			break;
		case VariantValue::type_float:
			result = VariantValue::Float(math::clamp(in[0].valueFloat(), in[1].toFloat(), in[2].toFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(math::clamp(in[0].valueFloat2(), in[1].toFloat2(), in[2].toFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(math::clamp(in[0].valueFloat3(), in[1].toFloat3(), in[2].toFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(math::clamp(in[0].valueFloat4(), in[1].toFloat4(), in[2].toFloat4()));
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
	switch(in[0].type()) {
		case VariantValue::type_float:
			result = VariantValue::Float(math::linearInterpolation(in[0].valueFloat(), in[1].toFloat(), in[2].toFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(math::linearInterpolation(in[0].valueFloat2(), in[1].toFloat2(), in[2].toFloat()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(math::linearInterpolation(in[0].valueFloat3(), in[1].toFloat3(), in[2].toFloat()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(math::linearInterpolation(in[0].valueFloat4(), in[1].toFloat4(), in[2].toFloat()));
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
	switch(in[0].type()) {
		case VariantValue::type_float:
			result = VariantValue::Float(math::floor(in[0].valueFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(math::floor(in[0].valueFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(math::floor(in[0].valueFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(math::floor(in[0].valueFloat4()));
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
	switch(in[0].type()) {
		case VariantValue::type_float:
			result = VariantValue::Float(math::ceil(in[0].valueFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(math::ceil(in[0].valueFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(math::ceil(in[0].valueFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(math::ceil(in[0].valueFloat4()));
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
	switch(in[0].type()) {
		case VariantValue::type_float:
			result = VariantValue::Float(math::round(in[0].valueFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(math::round(in[0].valueFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(math::round(in[0].valueFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(math::round(in[0].valueFloat4()));
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
	switch(in[0].type()) {
		case VariantValue::type_float:
			result = VariantValue::Float(math::sqrt(in[0].valueFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(math::sqrt(in[0].valueFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(math::sqrt(in[0].valueFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(math::sqrt(in[0].valueFloat4()));
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
	switch(in[0].type()) {
		case VariantValue::type_float:
			result = VariantValue::Float(math::sin(in[0].valueFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(math::sin(in[0].valueFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(math::sin(in[0].valueFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(math::sin(in[0].valueFloat4()));
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
	switch(in[0].type()) {
		case VariantValue::type_float:
			result = VariantValue::Float(math::cos(in[0].valueFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(math::cos(in[0].valueFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(math::cos(in[0].valueFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(math::cos(in[0].valueFloat4()));
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
	switch(in[0].type()) {
		case VariantValue::type_float:
			result = VariantValue::Float(math::asin(in[0].valueFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(math::asin(in[0].valueFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(math::asin(in[0].valueFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(math::asin(in[0].valueFloat4()));
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
	switch(in[0].type()) {
		case VariantValue::type_float:
			result = VariantValue::Float(math::acos(in[0].valueFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(math::acos(in[0].valueFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(math::acos(in[0].valueFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(math::acos(in[0].valueFloat4()));
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
	switch(in[0].type()) {
		case VariantValue::type_float2:
			result = VariantValue::Float(math::dot(in[0].valueFloat2(), in[1].toFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float(math::dot(in[0].valueFloat3(), in[1].toFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float(math::dot(in[0].valueFloat4(), in[1].toFloat4()));
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
	return std::vector<VariantValue>{ VariantValue::Float3(math::cross(in[0].toFloat3(), in[1].toFloat3())) };
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
	switch(in[0].type()) {
		case VariantValue::type_float2:
			result = VariantValue::Float2(math::normalize(in[0].valueFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(math::normalize(in[0].valueFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(math::normalize(in[0].valueFloat4()));
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
	switch(in[0].type()) {
		case VariantValue::type_float2:
			result = VariantValue::Float(math::length(in[0].valueFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float(math::length(in[0].valueFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float(math::length(in[0].valueFloat4()));
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
	switch(in[0].type()) {
		case VariantValue::type_float:
			result = VariantValue::Float(math::stepInterpolation(0.0, 1.0, in[1].toFloat(), in[0].valueFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(math::stepInterpolation(float2_t(0.0), float2_t(1.0), in[1].toFloat2(), in[0].valueFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(math::stepInterpolation(float3_t(0.0), float3_t(1.0), in[1].toFloat3(), in[0].valueFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(math::stepInterpolation(float4_t(0.0), float4_t(1.0), in[1].toFloat4(), in[0].valueFloat4()));
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
	switch(in[0].type()) {
		case VariantValue::type_float:
			result = VariantValue::Float(math::smoothstepInterpolation(0.0, 1.0, in[1].toFloat(), in[2].toFloat(), in[0].valueFloat()));
			break;
		case VariantValue::type_float2:
			result = VariantValue::Float2(math::smoothstepInterpolation(float2_t(0.0), float2_t(1.0), in[1].toFloat2(), in[2].toFloat2(), in[0].valueFloat2()));
			break;
		case VariantValue::type_float3:
			result = VariantValue::Float3(math::smoothstepInterpolation(float3_t(0.0), float3_t(1.0), in[1].toFloat3(), in[2].toFloat3(), in[0].valueFloat3()));
			break;
		case VariantValue::type_float4:
			result = VariantValue::Float4(math::smoothstepInterpolation(float4_t(0.0), float4_t(1.0), in[1].toFloat4(), in[2].toFloat4(), in[0].valueFloat4()));
			break;
		default:
			break;
	}

	return std::vector<VariantValue>{ result };
}
}
