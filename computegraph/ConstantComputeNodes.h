#pragma once

#include "ComputeNodeBase.h"

namespace pixelpart {
class BooleanConstantComputeNode : public ComputeNodeBase<BooleanConstantComputeNode> {
public:
	static std::string typeName;

	BooleanConstantComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class IntegerConstantComputeNode : public ComputeNodeBase<IntegerConstantComputeNode> {
public:
	static std::string typeName;

	IntegerConstantComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class NumberConstantComputeNode : public ComputeNodeBase<NumberConstantComputeNode> {
public:
	static std::string typeName;

	NumberConstantComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class Vector2ConstantComputeNode : public ComputeNodeBase<Vector2ConstantComputeNode> {
public:
	static std::string typeName;

	Vector2ConstantComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class Vector3ConstantComputeNode : public ComputeNodeBase<Vector3ConstantComputeNode> {
public:
	static std::string typeName;

	Vector3ConstantComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class Vector4ConstantComputeNode : public ComputeNodeBase<Vector4ConstantComputeNode> {
public:
	static std::string typeName;

	Vector4ConstantComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class ColorConstantComputeNode : public ComputeNodeBase<ColorConstantComputeNode> {
public:
	static std::string typeName;

	ColorConstantComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};
}