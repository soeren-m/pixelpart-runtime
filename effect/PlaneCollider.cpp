#include "PlaneCollider.h"

namespace pixelpart {
PlaneCollider::PlaneCollider(id_t ownId, id_t parentId) : Collider(ownId, parentId) {

}

ColliderType PlaneCollider::colliderType() const {
	return ColliderType::plane;
}

Node* PlaneCollider::cloneImpl() const {
	return new PlaneCollider(*this);
}

void to_json(nlohmann::ordered_json& j, const PlaneCollider& collider) {
	to_json(j, static_cast<const Collider&>(collider));

	j.update(nlohmann::ordered_json{
		{ "collider_type", ColliderType::plane }
	});
}
void from_json(const nlohmann::ordered_json& j, PlaneCollider& collider) {
	collider = PlaneCollider(j.at("id").get<id_t>());
	from_json(j, static_cast<Collider&>(collider));
}
}
