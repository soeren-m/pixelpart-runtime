#include "LineCollider.h"

namespace pixelpart {
LineCollider::LineCollider(id_t ownId, id_t parentId) : Collider(ownId, parentId) {

}

ColliderType LineCollider::colliderType() const {
	return ColliderType::line;
}

float3_t& LineCollider::point(std::size_t index) {
	return colliderPoints.at(index);
}
const float3_t& LineCollider::point(std::size_t index) const {
	return colliderPoints.at(index);
}
LineCollider::PointList& LineCollider::points() {
	return colliderPoints;
}
const LineCollider::PointList& LineCollider::points() const {
	return colliderPoints;
}

Node* LineCollider::cloneImpl() const {
	return new LineCollider(*this);
}

void to_json(nlohmann::ordered_json& j, const LineCollider& collider) {
	to_json(j, static_cast<const Collider&>(collider));

	j.update(nlohmann::ordered_json{
		{ "collider_type", ColliderType::line },
		{ "points", collider.points() }
	});
}
void from_json(const nlohmann::ordered_json& j, LineCollider& collider) {
	collider = LineCollider(j.at("id").get<id_t>());
	from_json(j, static_cast<Collider&>(collider));

	collider.points() = j.value("points", LineCollider::PointList{ float3_t(-0.5, 0.0, 0.0), float3_t(+0.5, 0.0, 0.0) });
}
}