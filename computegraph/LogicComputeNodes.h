#pragma once

#include "ComputeNodeBase.h"

namespace pixelpart {
class NegateComputeNode : public ComputeNodeBase<NegateComputeNode> {
public:
	static std::string typeName;

	NegateComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class AndComputeNode : public ComputeNodeBase<AndComputeNode> {
public:
	static std::string typeName;

	AndComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class OrComputeNode : public ComputeNodeBase<OrComputeNode> {
public:
	static std::string typeName;

	OrComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class XorComputeNode : public ComputeNodeBase<XorComputeNode> {
public:
	static std::string typeName;

	XorComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class BranchComputeNode : public ComputeNodeBase<BranchComputeNode> {
public:
	static std::string typeName;

	BranchComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};
}