#pragma once

#include "ComputeNode.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace pixelpart {
class ComputeNodeFactory {
public:
	ComputeNodeFactory() = default;

	std::unique_ptr<ComputeNode> create(const std::string& name) const;

	template <typename T>
	void registerNode() {
		if(nodeTypeFunctions.count(T::typeName) != 0) {
			return;
		}

		insertionOrder.push_back(T::typeName);
		nodeTypeFunctions[T::typeName] = []() -> ComputeNode* {
			return static_cast<ComputeNode*>(new T());
		};
	}

	void registerBuiltInNodes();

	std::vector<std::string> registeredTypes() const;

private:
	using CreateNodeFunction = std::function<ComputeNode*()>;

	std::unordered_map<std::string, CreateNodeFunction> nodeTypeFunctions;
	std::vector<std::string> insertionOrder;
};
}
