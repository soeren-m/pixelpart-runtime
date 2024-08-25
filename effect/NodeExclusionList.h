#pragma once

#include "../common/Types.h"
#include "Node.h"
#include <vector>

namespace pixelpart {
class NodeExclusionSet {
public:
	NodeExclusionList() = default;

	void addNode(id_t nodeId);
	void addNode(const Node& node);

private:
	std::vector<id_t> excludedNodeIds;
};
}

// TODO: json