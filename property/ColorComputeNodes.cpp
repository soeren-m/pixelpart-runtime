#include "ColorComputeNodes.h"

namespace pixelpart {
std::string BlendComputeNode::typeName = "compute_node_blend";
std::string ColorRampComputeNode::typeName = "compute_node_colorramp";
std::string BrightnessComputeNode::typeName = "compute_node_brightness";
std::string ExposureComputeNode::typeName = "compute_node_exposure";
std::string ContrastComputeNode::typeName = "compute_node_contrast";
std::string SaturationComputeNode::typeName = "compute_node_saturation";
std::string PosterizeComputeNode::typeName = "compute_node_posterize";

BlendComputeNode::BlendComputeNode() : ComputeNodeBase(typeName,
	"compute_category_color",
	{ "compute_slot_a", "compute_slot_b" },
	{ "compute_slot_color" },
	{ Signature{ { VariantValue::type_float4, VariantValue::type_float4 }, { VariantValue::type_float4 } } },
	{ VariantValue(), VariantValue() },
	{
		ComputeNodeParameter::createEnumParameter("compute_param_mode", 0, std::vector<std::string>{
			"blend_multiply",
			"blend_add",
			"blend_subtract",
			"blend_difference",
			"blend_screen",
			"blend_overlay",
			"blend_lighten",
			"blend_darken",
			"blend_coloronly"})
	}) {

}
std::vector<VariantValue> BlendComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	VariantValue result = VariantValue::Float4(vec4d(1.0));
	switch(parameterValues[0].getEnum()) {
		case 0:
			result = VariantValue::Float4(in[0].toFloat4() * in[1].toFloat4());
			break;
		case 1:
			result = VariantValue::Float4(vec4d(
				glm::clamp(in[0].toFloat3() + in[1].toFloat3(), vec3d(0.0), vec3d(1.0)),
				in[0].toFloat4().a * in[1].toFloat4().a));
			break;
		case 2:
			result = VariantValue::Float4(vec4d(
				glm::clamp(in[0].toFloat3() - in[1].toFloat3(), vec3d(0.0), vec3d(1.0)),
				in[0].toFloat4().a * in[1].toFloat4().a));
			break;
		case 3:
			result = VariantValue::Float4(vec4d(
				glm::abs(in[0].toFloat3() - in[1].toFloat3()),
				in[0].toFloat4().a * in[1].toFloat4().a));
			break;
		case 4:
			result = VariantValue::Float4(vec4d(
				vec3d(1.0) - (vec3d(1.0) - in[0].toFloat3()) * (vec3d(1.0) - in[1].toFloat3()),
				in[0].toFloat4().a * in[1].toFloat4().a));
			break;
		case 5:
			result = VariantValue::Float4(vec4d(
				(in[0].toFloat4().r < 0.5) ? (2.0 * in[0].toFloat4().r * in[1].toFloat4().r) : (1.0 - 2.0 * (1.0 - in[0].toFloat4().r) * (1.0 - in[1].toFloat4().r)),
				(in[0].toFloat4().g < 0.5) ? (2.0 * in[0].toFloat4().g * in[1].toFloat4().g) : (1.0 - 2.0 * (1.0 - in[0].toFloat4().g) * (1.0 - in[1].toFloat4().g)),
				(in[0].toFloat4().b < 0.5) ? (2.0 * in[0].toFloat4().b * in[1].toFloat4().b) : (1.0 - 2.0 * (1.0 - in[0].toFloat4().b) * (1.0 - in[1].toFloat4().b)),
				in[0].toFloat4().a * in[1].toFloat4().a));
			break;
		case 6:
			result = VariantValue::Float4(vec4d(
				glm::max(in[0].toFloat3(), in[1].toFloat3()),
				in[0].toFloat4().a * in[1].toFloat4().a));
			break;
		case 7:
			result = VariantValue::Float4(vec4d(
				glm::min(in[0].toFloat3(), in[1].toFloat3()),
				in[0].toFloat4().a * in[1].toFloat4().a));
			break;
		case 8:
			result = VariantValue::Float4(vec4d(
				in[1].toFloat3(),
				in[0].toFloat4().a * in[1].toFloat4().a));
			break;
	}

	return std::vector<VariantValue>{ result };
}

ColorRampComputeNode::ColorRampComputeNode() : ComputeNodeBase(typeName,
	"compute_category_color",
	{ "compute_slot_value" },
	{ "compute_slot_color" },
	{ Signature{ { VariantValue::type_float }, { VariantValue::type_float4 } } },
	{ VariantValue::Float(0.0) },
	{ ComputeNodeParameter::createGradientParameter("compute_param_gradient", Curve<vec4d>(vec4d(0.5), CurveInterpolation::linear)) }) {

}
std::vector<VariantValue> ColorRampComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{
		VariantValue::Float4(parameterValues[0].getGradient4().get(in[0].toFloat()))
	};
}

BrightnessComputeNode::BrightnessComputeNode() : ComputeNodeBase(typeName,
	"compute_category_color",
	{ "compute_slot_color", "compute_slot_brightness" },
	{ "compute_slot_color" },
	{ Signature{ { VariantValue::type_float4, VariantValue::type_float }, { VariantValue::type_float4 } } },
	{ VariantValue(), VariantValue::Float(0.0) },
	{ }) {

}
std::vector<VariantValue> BrightnessComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{
		VariantValue::Float4(vec4d(
			in[0].toFloat3() + vec3d(in[1].toFloat()),
			in[0].toFloat4().a))
	};
}

ExposureComputeNode::ExposureComputeNode() : ComputeNodeBase(typeName,
	"compute_category_color",
	{ "compute_slot_color", "compute_slot_exposure" },
	{ "compute_slot_color" },
	{ Signature{ { VariantValue::type_float4, VariantValue::type_float }, { VariantValue::type_float4 } } },
	{ VariantValue(), VariantValue::Float(0.0) },
	{ }) {

}
std::vector<VariantValue> ExposureComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{
		VariantValue::Float4(vec4d(
			in[0].toFloat3() * vec3d(1.0 + in[1].toFloat()),
			in[0].toFloat4().a))
	};
}

ContrastComputeNode::ContrastComputeNode() : ComputeNodeBase(typeName,
	"compute_category_color",
	{ "compute_slot_color", "compute_slot_contrast" },
	{ "compute_slot_color" },
	{ Signature{ { VariantValue::type_float4, VariantValue::type_float }, { VariantValue::type_float4 } } },
	{ VariantValue(), VariantValue::Float(0.0) },
	{ }) {

}
std::vector<VariantValue> ContrastComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{
		VariantValue::Float4(vec4d(
			vec3d(0.5) + (in[0].toFloat3() - vec3d(0.5)) * (1.0 + in[1].toFloat()),
			in[0].toFloat4().a))
	};
}

SaturationComputeNode::SaturationComputeNode() : ComputeNodeBase(typeName,
	"compute_category_color",
	{ "compute_slot_color", "compute_slot_saturation" },
	{ "compute_slot_color" },
	{ Signature{ { VariantValue::type_float4, VariantValue::type_float }, { VariantValue::type_float4 } } },
	{ VariantValue(), VariantValue::Float(0.0) },
	{ }) {

}
std::vector<VariantValue> SaturationComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{
		VariantValue::Float4(vec4d(
			glm::mix(
				vec3d(glm::dot(in[0].toFloat3(), vec3d(0.2126, 0.7152, 0.0722))),
				in[0].toFloat3(),
				1.0 + in[1].toFloat()),
			in[0].toFloat4().a))
	};
}

PosterizeComputeNode::PosterizeComputeNode() : ComputeNodeBase(typeName,
	"compute_category_color",
	{ "compute_slot_color", "compute_slot_number" },
	{ "compute_slot_color" },
	{ Signature{ { VariantValue::type_float4, VariantValue::type_int }, { VariantValue::type_float4 } } },
	{ VariantValue(), VariantValue::Int(256) },
	{ }) {

}
std::vector<VariantValue> PosterizeComputeNode::evaluate(const std::vector<VariantValue>& in) const {
	return std::vector<VariantValue>{
		VariantValue::Float4(glm::round(in[0].toFloat4() * in[1].toFloat()) / in[1].toFloat())
	};
}
}