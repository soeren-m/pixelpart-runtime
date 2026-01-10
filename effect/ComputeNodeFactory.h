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
	ComputeNodeFactory();

	std::unique_ptr<ComputeNode> createNode(const std::string& name) const;

	std::vector<std::string> nodeTypeNames() const;

private:
	using CreateNodeFunction = std::function<ComputeNode*()>;

	template <typename T>
	void registerNode() {
		if(factoryNodeFunctions.count(T::typeName) != 0) {
			return;
		}

		factoryNodeTypeNames.push_back(T::typeName);
		factoryNodeFunctions[T::typeName] = []() -> ComputeNode* {
			return static_cast<ComputeNode*>(new T());
		};
	}

	std::unordered_map<std::string, CreateNodeFunction> factoryNodeFunctions;
	std::vector<std::string> factoryNodeTypeNames;
};
}
