#pragma once

#include "ComputeNodeBase.h"
#include <string>

namespace pixelpart {
class CurveComputeNode : public ComputeNodeBase<CurveComputeNode> {
public:
	static std::string typeName;

	CurveComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class Split2ComputeNode : public ComputeNodeBase<Split2ComputeNode> {
public:
	static std::string typeName;

	Split2ComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class Split3ComputeNode : public ComputeNodeBase<Split3ComputeNode> {
public:
	static std::string typeName;

	Split3ComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class Split4ComputeNode : public ComputeNodeBase<Split4ComputeNode> {
public:
	static std::string typeName;

	Split4ComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class Merge2ComputeNode : public ComputeNodeBase<Merge2ComputeNode> {
public:
	static std::string typeName;

	Merge2ComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class Merge3ComputeNode : public ComputeNodeBase<Merge3ComputeNode> {
public:
	static std::string typeName;

	Merge3ComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class Merge4ComputeNode : public ComputeNodeBase<Merge4ComputeNode> {
public:
	static std::string typeName;

	Merge4ComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};
}
