#pragma once

#include "ComputeNode.h"

namespace pixelpart {
template <typename TDerived>
class ComputeNodeBase : public ComputeNode {
public:
	ComputeNodeBase(
		const std::string& nodeType,
		const std::string& nodeCategory,
		const std::vector<std::string>& nodeInputs,
		const std::vector<std::string>& nodeOutputs,
		const std::vector<Signature>& nodeSignatures,
		const std::vector<VariantValue>& nodeDefaultInputs,
		const std::vector<VariantParameter>& nodeParameters) :
		ComputeNode(nodeType, nodeCategory, nodeInputs, nodeOutputs, nodeSignatures, nodeDefaultInputs, nodeParameters) {

	}

protected:
	virtual ComputeNode* cloneImpl() const override {
		return new TDerived();
	}
};
}