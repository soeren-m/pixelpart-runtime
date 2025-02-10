#pragma once

#include "ComputeNodeBase.h"
#include <string>

namespace pixelpart {
class EqualComputeNode : public ComputeNodeBase<EqualComputeNode> {
public:
	static std::string typeName;

	EqualComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class NotEqualComputeNode : public ComputeNodeBase<NotEqualComputeNode> {
public:
	static std::string typeName;

	NotEqualComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class ApproxEqualComputeNode : public ComputeNodeBase<ApproxEqualComputeNode> {
public:
	static std::string typeName;

	ApproxEqualComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class LessComputeNode : public ComputeNodeBase<LessComputeNode> {
public:
	static std::string typeName;

	LessComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class LessOrEqualComputeNode : public ComputeNodeBase<LessOrEqualComputeNode> {
public:
	static std::string typeName;

	LessOrEqualComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class GreaterComputeNode : public ComputeNodeBase<GreaterComputeNode> {
public:
	static std::string typeName;

	GreaterComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class GreaterOrEqualComputeNode : public ComputeNodeBase<GreaterOrEqualComputeNode> {
public:
	static std::string typeName;

	GreaterOrEqualComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};
}