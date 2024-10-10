#pragma once

#include "ComputeNode.h"

namespace pixelpart {
template <typename TDerived>
class ComputeNodeBase : public ComputeNode {
public:
	ComputeNodeBase(
		const std::string& type,
		const std::string& category,
		const std::vector<std::string>& inputSlots,
		const std::vector<std::string>& outputSlots,
		const std::vector<Signature>& signatures,
		const std::vector<VariantValue>& defaultInputs,
		const std::vector<VariantParameter>& parameters) :
		ComputeNode(type, category, inputSlots, outputSlots, signatures, defaultInputs, parameters) {

	}

protected:
	virtual ComputeNode* cloneImpl() const override {
		return new TDerived();
	}
};
}