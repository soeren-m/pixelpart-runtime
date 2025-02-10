#pragma once

#include "Node.h"
#include "../common/Id.h"
#include "../json/json.hpp"

namespace pixelpart {
class GroupNode : public Node {
public:
	GroupNode() = default;
	GroupNode(id_t ownId, id_t parentId = id_t());

protected:
	virtual Node* cloneImpl() const override;
};

void to_json(nlohmann::ordered_json& j, const GroupNode& groupNode);
void from_json(const nlohmann::ordered_json& j, GroupNode& groupNode);
}