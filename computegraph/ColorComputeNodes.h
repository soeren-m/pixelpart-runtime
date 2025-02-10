#pragma once

#include "ComputeNodeBase.h"
#include <string>

namespace pixelpart {
class BlendComputeNode : public ComputeNodeBase<BlendComputeNode> {
public:
	static std::string typeName;

	BlendComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class ColorRampComputeNode : public ComputeNodeBase<ColorRampComputeNode> {
public:
	static std::string typeName;

	ColorRampComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class BrightnessComputeNode : public ComputeNodeBase<BrightnessComputeNode> {
public:
	static std::string typeName;

	BrightnessComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class ExposureComputeNode : public ComputeNodeBase<ExposureComputeNode> {
public:
	static std::string typeName;

	ExposureComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class ContrastComputeNode : public ComputeNodeBase<ContrastComputeNode> {
public:
	static std::string typeName;

	ContrastComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class SaturationComputeNode : public ComputeNodeBase<SaturationComputeNode> {
public:
	static std::string typeName;

	SaturationComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};

class PosterizeComputeNode : public ComputeNodeBase<PosterizeComputeNode> {
public:
	static std::string typeName;

	PosterizeComputeNode();
	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};
}