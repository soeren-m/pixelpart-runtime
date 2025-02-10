#pragma once

#include "ComputeNodeBase.h"
#include <string>

namespace pixelpart {
class OutputComputeNode : public ComputeNodeBase<OutputComputeNode> {
public:
	static std::string typeName;

	OutputComputeNode();

	virtual std::vector<VariantValue> evaluate(const std::vector<VariantValue>& in) const override;
};
}