#pragma once

#include "ComputeNode.h"
#include <functional>

namespace pixelpart {
class ComputeNodeFactory {
public:
	ComputeNodeFactory();

	template <typename T>
	void registerNode() {
		if(nodeTypes.count(T::typeName) != 0u) {
			return;
		}

		insertionOrder.push_back(T::typeName);
		nodeTypes[T::typeName] = []() -> ComputeNode* {
			return static_cast<ComputeNode*>(new T());
		};
	}

	void registerBuiltInNodes();

	std::vector<std::string> getRegisteredTypes() const;

	std::unique_ptr<ComputeNode> create(const std::string& name) const;

private:
	using CreateNodeFunction = std::function<ComputeNode*()>;

	std::unordered_map<std::string, CreateNodeFunction> nodeTypes;
	std::vector<std::string> insertionOrder;
};
}