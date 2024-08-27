#pragma once

#include "../common/Types.h"
#include "Node.h"
#include "../json/json.hpp"
#include <unordered_set>

namespace pixelpart {
class NodeExclusionSet {
public:
	NodeExclusionSet() = default;
	NodeExclusionSet(const std::unordered_set<id_t>& nodeIds);

	void add(id_t nodeId);
	void add(const Node& node);

	void remove(id_t nodeId);
	void remove(const Node& node);

	bool excluded(id_t nodeId) const;
	bool excluded(const Node& node) const;

	const std::unordered_set<id_t>& nodeIds() const;

private:
	std::unordered_set<id_t> excludedNodeIds;
};

void to_json(nlohmann::ordered_json& j, const NodeExclusionSet& set);
void from_json(const nlohmann::ordered_json& j, NodeExclusionSet& set);
}