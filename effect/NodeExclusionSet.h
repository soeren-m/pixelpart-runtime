#pragma once

#include "../common/Types.h"
#include "../common/Id.h"
#include "Node.h"
#include "../json/json.hpp"
#include <set>

namespace pixelpart {
class NodeExclusionSet {
public:
	using iterator = typename std::set<id_t>::iterator;
	using const_iterator = typename std::set<id_t>::const_iterator;

	NodeExclusionSet() = default;
	NodeExclusionSet(const std::set<id_t>& nodeIds);

	iterator begin();
	iterator end();

	const_iterator begin() const;
	const_iterator end() const;

	void add(id_t nodeId);
	void add(const Node& node);

	void remove(id_t nodeId);
	void remove(const Node& node);

	bool excluded(id_t nodeId) const;
	bool excluded(const Node& node) const;

	std::size_t count() const;

	const std::set<id_t>& nodeIds() const;

private:
	std::set<id_t> excludedNodeIds;
};

void to_json(nlohmann::ordered_json& j, const NodeExclusionSet& set);
void from_json(const nlohmann::ordered_json& j, NodeExclusionSet& set);
}