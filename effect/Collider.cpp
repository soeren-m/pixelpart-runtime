#include "Collider.h"
#include "NodeType.h"

namespace pixelpart {
Collider::Collider(id_t ownId, id_t parentId) : Node(ownId, parentId) {

}

void Collider::applyInputs(const ComputeGraph::InputSet& inputs) {
	Node::applyInputs(inputs);

	colliderKillOnContact.input(inputs);
	colliderBounce.input(inputs);
	colliderFriction.input(inputs);
}

std::set<id_t>& Collider::exclusionSet() {
	return colliderExclusionSet;
}
const std::set<id_t>& Collider::exclusionSet() const {
	return colliderExclusionSet;
}

StaticProperty<bool>& Collider::killOnContact() {
	return colliderKillOnContact;
}
const StaticProperty<bool>& Collider::killOnContact() const {
	return colliderKillOnContact;
}

AnimatedProperty<float_t>& Collider::bounce() {
	return colliderBounce;
}
const AnimatedProperty<float_t>& Collider::bounce() const {
	return colliderBounce;
}

AnimatedProperty<float_t>& Collider::friction() {
	return colliderFriction;
}
const AnimatedProperty<float_t>& Collider::friction() const {
	return colliderFriction;
}

void to_json(nlohmann::ordered_json& j, const Collider& collider) {
	to_json(j, static_cast<const Node&>(collider));

	j.update(nlohmann::ordered_json{
		{ "node_type", NodeType::collider },
		{ "exclusion_list", collider.exclusionSet() },
		{ "kill_on_contact", collider.killOnContact() },
		{ "bounce", collider.bounce() },
		{ "friction", collider.friction() }
	});
}
void from_json(const nlohmann::ordered_json& j, Collider& collider) {
	from_json(j, static_cast<Node&>(collider));

	collider.exclusionSet() = j.value("exclusion_list", std::set<id_t>());
	collider.killOnContact() = j.value("kill_on_contact", StaticProperty<bool>(false));
	collider.bounce() = j.value("bounce", AnimatedProperty<float_t>(0.5));
	collider.friction() = j.value("friction", AnimatedProperty<float_t>(0.5));
}
}