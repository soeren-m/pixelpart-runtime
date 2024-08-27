#include "Collider.h"

namespace pixelpart {
Collider::Collider(id_t ownId, id_t parentId) : Node(ownId, parentId) {

}

NodeExclusionSet& Collider::exclusionSet() {
	return colliderExclusionSet;
}
const NodeExclusionSet& Collider::exclusionSet() const {
	return colliderExclusionSet;
}

Collider::PointList& Collider::points() {
	return colliderPoints;
}
const Collider::PointList& Collider::points() const {
	return colliderPoints;
}

StaticProperty<float_t>& Collider::width() {
	return colliderWidth;
}
const StaticProperty<float_t>& Collider::width() const {
	return colliderWidth;
}

StaticProperty<float_t>& Collider::orientation() {
	return colliderOrientation;
}
const StaticProperty<float_t>& Collider::orientation() const {
	return colliderOrientation;
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
	j = nlohmann::ordered_json{
		{ "id", toJson(collider.id) },
		{ "parent_id", toJson(collider.parentId) },
		{ "name", collider.name() },
		{ "lifetime_start", collider.start() },
		{ "lifetime_duration", collider.duration() },
		{ "repeat", collider.repeat() },
		{ "position", collider.position() },

		{ "exclusion_list", collider.exclusionSet() },
		{ "points", collider.points() },
		{ "width", collider.width() },
		{ "orientation", collider.orientation() },
		{ "kill_on_contact", collider.killOnContact() },
		{ "bounce", collider.bounce() },
		{ "friction", collider.friction() }
	};
}
void from_json(const nlohmann::ordered_json& j, Collider& collider) {
	collider = Collider(
		j.at("id"),
		j.value("parent_id", nullId));

	collider.name(j.value("name", ""));
	collider.start(j.value("lifetime_start", 0.0));
	collider.duration(j.value("lifetime_duration", 1.0));
	collider.repeat(j.value("repeat", true));
	collider.position() = j.value("position", AnimatedProperty<vec3_t>(0.0, vec3_t(0.0)));

	collider.exclusionSet() = j.value("exclusion_list", NodeExclusionSet());
	collider.points() = j.value("points", Collider::PointList{ vec3_t(-0.5, 0.0, 0.0), vec3_t(+0.5, 0.0, 0.0) });
	collider.width() = j.value("width", StaticProperty<float_t>(1.0));
	collider.orientation() = j.value("orientation", StaticProperty<float_t>(0.0));
	collider.killOnContact() = j.value("kill_on_contact", StaticProperty<bool>(false));
	collider.bounce() = j.value("bounce", AnimatedProperty<float_t>(0.5));
	collider.friction() = j.value("friction", AnimatedProperty<float_t>(0.5));
}
}