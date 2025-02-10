#pragma once

#include "ComputeNodeBase.h"
#include <string>

namespace pixelpart {
class AddComputeNode : public ComputeNodeBase<AddComputeNode> {
public:
	static std::string typeName;

	AddComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class SubtractComputeNode : public ComputeNodeBase<SubtractComputeNode> {
public:
	static std::string typeName;

	SubtractComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class MultiplyComputeNode : public ComputeNodeBase<MultiplyComputeNode> {
public:
	static std::string typeName;

	MultiplyComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class DivideComputeNode : public ComputeNodeBase<DivideComputeNode> {
public:
	static std::string typeName;

	DivideComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class ModComputeNode : public ComputeNodeBase<ModComputeNode> {
public:
	static std::string typeName;

	ModComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class PowComputeNode : public ComputeNodeBase<PowComputeNode> {
public:
	static std::string typeName;

	PowComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class ExpComputeNode : public ComputeNodeBase<ExpComputeNode> {
public:
	static std::string typeName;

	ExpComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class LogComputeNode : public ComputeNodeBase<LogComputeNode> {
public:
	static std::string typeName;

	LogComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class SignComputeNode : public ComputeNodeBase<SignComputeNode> {
public:
	static std::string typeName;

	SignComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class AbsComputeNode : public ComputeNodeBase<AbsComputeNode> {
public:
	static std::string typeName;

	AbsComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class MinComputeNode : public ComputeNodeBase<MinComputeNode> {
public:
	static std::string typeName;

	MinComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class MaxComputeNode : public ComputeNodeBase<MaxComputeNode> {
public:
	static std::string typeName;

	MaxComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class ClampComputeNode : public ComputeNodeBase<ClampComputeNode> {
public:
	static std::string typeName;

	ClampComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class LerpComputeNode : public ComputeNodeBase<LerpComputeNode> {
public:
	static std::string typeName;

	LerpComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class FloorComputeNode : public ComputeNodeBase<FloorComputeNode> {
public:
	static std::string typeName;

	FloorComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class CeilComputeNode : public ComputeNodeBase<CeilComputeNode> {
public:
	static std::string typeName;

	CeilComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class RoundComputeNode : public ComputeNodeBase<RoundComputeNode> {
public:
	static std::string typeName;

	RoundComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class SqrtComputeNode : public ComputeNodeBase<SqrtComputeNode> {
public:
	static std::string typeName;

	SqrtComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class SinComputeNode : public ComputeNodeBase<SinComputeNode> {
public:
	static std::string typeName;

	SinComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class CosComputeNode : public ComputeNodeBase<CosComputeNode> {
public:
	static std::string typeName;

	CosComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class AsinComputeNode : public ComputeNodeBase<AsinComputeNode> {
public:
	static std::string typeName;

	AsinComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class AcosComputeNode : public ComputeNodeBase<AcosComputeNode> {
public:
	static std::string typeName;

	AcosComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class DotComputeNode : public ComputeNodeBase<DotComputeNode> {
public:
	static std::string typeName;

	DotComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class CrossComputeNode : public ComputeNodeBase<CrossComputeNode> {
public:
	static std::string typeName;

	CrossComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class NormalizeComputeNode : public ComputeNodeBase<NormalizeComputeNode> {
public:
	static std::string typeName;

	NormalizeComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class VectorLengthComputeNode : public ComputeNodeBase<VectorLengthComputeNode> {
public:
	static std::string typeName;

	VectorLengthComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class StepComputeNode : public ComputeNodeBase<StepComputeNode> {
public:
	static std::string typeName;

	StepComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class SmoothstepComputeNode : public ComputeNodeBase<SmoothstepComputeNode> {
public:
	static std::string typeName;

	SmoothstepComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};
}