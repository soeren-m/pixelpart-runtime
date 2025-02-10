#pragma once

#include "Collider.h"
#include "../common/Math.h"
#include "../json/json.hpp"
#include <vector>

namespace pixelpart {
class LineCollider : public Collider {
public:
	using PointList = std::vector<float3_t>;

	LineCollider() = default;
	LineCollider(id_t ownId, id_t parentId = id_t());

	virtual ColliderType colliderType() const override;

	float3_t& point(std::size_t index);
	const float3_t& point(std::size_t index) const;
	PointList& points();
	const PointList& points() const;

protected:
	virtual Node* cloneImpl() const override;

private:
	PointList colliderPoints = PointList{
		float3_t(-0.5, 0.0, 0.0),
		float3_t(+0.5, 0.0, 0.0)
	};
};

void to_json(nlohmann::ordered_json& j, const LineCollider& collider);
void from_json(const nlohmann::ordered_json& j, LineCollider& collider);
}