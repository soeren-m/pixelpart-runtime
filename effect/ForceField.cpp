#include "ForceField.h"
#include "NodeType.h"

namespace pixelpart {
ForceField::ForceField(id_t ownId, id_t parentId) : Node(ownId, parentId) {

}

void ForceField::applyInputs(const ComputeGraph::InputSet& inputs) {
	Node::applyInputs(inputs);

	fieldStrength.input(inputs);
}

void ForceField::infinite(bool mode) {
	fieldInfinite = mode;
}
bool ForceField::infinite() const {
	return fieldInfinite;
}

std::set<id_t>& ForceField::exclusionSet() {
	return fieldExclusionSet;
}
const std::set<id_t>& ForceField::exclusionSet() const {
	return fieldExclusionSet;
}

AnimatedProperty<float_t>& ForceField::strength() {
	return fieldStrength;
}
const AnimatedProperty<float_t>& ForceField::strength() const {
	return fieldStrength;
}

void to_json(nlohmann::ordered_json& j, const ForceField& forceField) {
	to_json(j, static_cast<const Node&>(forceField));

	j.update(nlohmann::ordered_json{
		{ "node_type", NodeType::force_field },
		{ "infinite", forceField.infinite() },
		{ "exclusion_list", forceField.exclusionSet() },
		{ "strength", forceField.strength() },
	});
}
void from_json(const nlohmann::ordered_json& j, ForceField& forceField) {
	from_json(j, static_cast<Node&>(forceField));

	forceField.infinite(j.value("infinite", false));
	forceField.exclusionSet() = j.value("exclusion_list", std::set<id_t>());
	forceField.strength() = j.value("strength", AnimatedProperty<float_t>(1.0));
}
}