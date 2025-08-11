#include "GroupNode.h"
#include "NodeType.h"

namespace pixelpart {
GroupNode::GroupNode(id_t ownId, id_t parentId) : Node(ownId, parentId) {

}

Node* GroupNode::cloneImpl() const {
	return new GroupNode(*this);
}

void to_json(nlohmann::ordered_json& j, const GroupNode& groupNode) {
	to_json(j, static_cast<const Node&>(groupNode));

	j.update(nlohmann::ordered_json{
		{ "node_type", NodeType::group }
	});
}
void from_json(const nlohmann::ordered_json& j, GroupNode& groupNode) {
	groupNode = GroupNode(j.at("id").get<id_t>());
	from_json(j, static_cast<Node&>(groupNode));
}
}
