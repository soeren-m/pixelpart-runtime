#include "ComputeNodeFactory.h"
#include "OutputComputeNode.h"
#include "ConstantComputeNodes.h"
#include "MathComputeNodes.h"
#include "ComparisonComputeNodes.h"
#include "LogicComputeNodes.h"
#include "ColorComputeNodes.h"
#include "UtilComputeNodes.h"

namespace pixelpart {
ComputeNodeFactory::ComputeNodeFactory() {

}

void ComputeNodeFactory::registerBuiltInNodes() {
	registerNode<pixelpart::OutputComputeNode>();
	registerNode<pixelpart::BooleanConstantComputeNode>();
	registerNode<pixelpart::IntegerConstantComputeNode>();
	registerNode<pixelpart::NumberConstantComputeNode>();
	registerNode<pixelpart::Vector2ConstantComputeNode>();
	registerNode<pixelpart::Vector3ConstantComputeNode>();
	registerNode<pixelpart::Vector4ConstantComputeNode>();
	registerNode<pixelpart::ColorConstantComputeNode>();
	registerNode<pixelpart::AddComputeNode>();
	registerNode<pixelpart::SubtractComputeNode>();
	registerNode<pixelpart::MultiplyComputeNode>();
	registerNode<pixelpart::DivideComputeNode>();
	registerNode<pixelpart::ModComputeNode>();
	registerNode<pixelpart::PowComputeNode>();
	registerNode<pixelpart::ExpComputeNode>();
	registerNode<pixelpart::LogComputeNode>();
	registerNode<pixelpart::SignComputeNode>();
	registerNode<pixelpart::AbsComputeNode>();
	registerNode<pixelpart::MinComputeNode>();
	registerNode<pixelpart::MaxComputeNode>();
	registerNode<pixelpart::ClampComputeNode>();
	registerNode<pixelpart::LerpComputeNode>();
	registerNode<pixelpart::FloorComputeNode>();
	registerNode<pixelpart::CeilComputeNode>();
	registerNode<pixelpart::RoundComputeNode>();
	registerNode<pixelpart::SqrtComputeNode>();
	registerNode<pixelpart::SinComputeNode>();
	registerNode<pixelpart::CosComputeNode>();
	registerNode<pixelpart::AsinComputeNode>();
	registerNode<pixelpart::AcosComputeNode>();
	registerNode<pixelpart::DotComputeNode>();
	registerNode<pixelpart::CrossComputeNode>();
	registerNode<pixelpart::NormalizeComputeNode>();
	registerNode<pixelpart::VectorLengthComputeNode>();
	registerNode<pixelpart::StepComputeNode>();
	registerNode<pixelpart::SmoothstepComputeNode>();
	registerNode<pixelpart::EqualComputeNode>();
	registerNode<pixelpart::NotEqualComputeNode>();
	registerNode<pixelpart::ApproxEqualComputeNode>();
	registerNode<pixelpart::LessComputeNode>();
	registerNode<pixelpart::LessOrEqualComputeNode>();
	registerNode<pixelpart::GreaterComputeNode>();
	registerNode<pixelpart::GreaterOrEqualComputeNode>();
	registerNode<pixelpart::NegateComputeNode>();
	registerNode<pixelpart::AndComputeNode>();
	registerNode<pixelpart::OrComputeNode>();
	registerNode<pixelpart::XorComputeNode>();
	registerNode<pixelpart::BranchComputeNode>();
	registerNode<pixelpart::BlendComputeNode>();
	registerNode<pixelpart::ColorRampComputeNode>();
	registerNode<pixelpart::BrightnessComputeNode>();
	registerNode<pixelpart::ExposureComputeNode>();
	registerNode<pixelpart::ContrastComputeNode>();
	registerNode<pixelpart::SaturationComputeNode>();
	registerNode<pixelpart::PosterizeComputeNode>();
	registerNode<pixelpart::CurveComputeNode>();
	registerNode<pixelpart::Split2ComputeNode>();
	registerNode<pixelpart::Split3ComputeNode>();
	registerNode<pixelpart::Split4ComputeNode>();
	registerNode<pixelpart::Merge2ComputeNode>();
	registerNode<pixelpart::Merge3ComputeNode>();
	registerNode<pixelpart::Merge4ComputeNode>();
}

std::vector<std::string> ComputeNodeFactory::getRegisteredTypes() const {
	return insertionOrder;
}

std::unique_ptr<ComputeNode> ComputeNodeFactory::create(const std::string& name) const {
	if(nodeTypes.count(name) == 0u) {
		return nullptr;
	}

	return std::unique_ptr<ComputeNode>(nodeTypes.at(name)());
}
}