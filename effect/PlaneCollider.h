#pragma once

#include "Collider.h"
#include "../json/json.hpp"

namespace pixelpart {
class PlaneCollider : public Collider {
public:
	using PointList = std::vector<float3_t>;

	PlaneCollider() = default;
	PlaneCollider(id_t ownId, id_t parentId = id_t());

	virtual ColliderType colliderType() const override;

protected:
	virtual Node* cloneImpl() const override;
};

void to_json(nlohmann::ordered_json& j, const PlaneCollider& collider);
void from_json(const nlohmann::ordered_json& j, PlaneCollider& collider);
}
