#include "ComputeNodeFactory.h"
#include "OutputComputeNode.h"
#include "ConstantComputeNodes.h"
#include "MathComputeNodes.h"
#include "ComparisonComputeNodes.h"
#include "LogicComputeNodes.h"
#include "ColorComputeNodes.h"
#include "UtilComputeNodes.h"

namespace pixelpart {
void ComputeNodeFactory::registerBuiltInNodes() {
	registerNode<OutputComputeNode>();
	registerNode<BooleanConstantComputeNode>();
	registerNode<IntegerConstantComputeNode>();
	registerNode<NumberConstantComputeNode>();
	registerNode<Vector2ConstantComputeNode>();
	registerNode<Vector3ConstantComputeNode>();
	registerNode<Vector4ConstantComputeNode>();
	registerNode<ColorConstantComputeNode>();
	registerNode<AddComputeNode>();
	registerNode<SubtractComputeNode>();
	registerNode<MultiplyComputeNode>();
	registerNode<DivideComputeNode>();
	registerNode<ModComputeNode>();
	registerNode<PowComputeNode>();
	registerNode<ExpComputeNode>();
	registerNode<LogComputeNode>();
	registerNode<SignComputeNode>();
	registerNode<AbsComputeNode>();
	registerNode<MinComputeNode>();
	registerNode<MaxComputeNode>();
	registerNode<ClampComputeNode>();
	registerNode<LerpComputeNode>();
	registerNode<FloorComputeNode>();
	registerNode<CeilComputeNode>();
	registerNode<RoundComputeNode>();
	registerNode<SqrtComputeNode>();
	registerNode<SinComputeNode>();
	registerNode<CosComputeNode>();
	registerNode<AsinComputeNode>();
	registerNode<AcosComputeNode>();
	registerNode<DotComputeNode>();
	registerNode<CrossComputeNode>();
	registerNode<NormalizeComputeNode>();
	registerNode<VectorLengthComputeNode>();
	registerNode<StepComputeNode>();
	registerNode<SmoothstepComputeNode>();
	registerNode<EqualComputeNode>();
	registerNode<NotEqualComputeNode>();
	registerNode<ApproxEqualComputeNode>();
	registerNode<LessComputeNode>();
	registerNode<LessOrEqualComputeNode>();
	registerNode<GreaterComputeNode>();
	registerNode<GreaterOrEqualComputeNode>();
	registerNode<NegateComputeNode>();
	registerNode<AndComputeNode>();
	registerNode<OrComputeNode>();
	registerNode<XorComputeNode>();
	registerNode<BranchComputeNode>();
	registerNode<BlendComputeNode>();
	registerNode<ColorRampComputeNode>();
	registerNode<BrightnessComputeNode>();
	registerNode<ExposureComputeNode>();
	registerNode<ContrastComputeNode>();
	registerNode<SaturationComputeNode>();
	registerNode<PosterizeComputeNode>();
	registerNode<CurveComputeNode>();
	registerNode<Split2ComputeNode>();
	registerNode<Split3ComputeNode>();
	registerNode<Split4ComputeNode>();
	registerNode<Merge2ComputeNode>();
	registerNode<Merge3ComputeNode>();
	registerNode<Merge4ComputeNode>();
}

std::unique_ptr<ComputeNode> ComputeNodeFactory::create(const std::string& name) const {
	if(nodeTypeFunctions.count(name) == 0) {
		return nullptr;
	}

	return std::unique_ptr<ComputeNode>(nodeTypeFunctions.at(name)());
}

std::vector<std::string> ComputeNodeFactory::registeredTypes() const {
	return insertionOrder;
}
}
