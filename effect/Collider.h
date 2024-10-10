#pragma once

#include "Node.h"
#include "../common/Types.h"
#include "../common/Math.h"
#include "../common/Id.h"
#include "NodeExclusionSet.h"
#include "StaticProperty.h"
#include "AnimatedProperty.h"
#include "../json/json.hpp"
#include <vector>

namespace pixelpart {
class Collider : public Node {
public:
	using PointList = std::vector<float3_t>;

	Collider() = default;
	Collider(id_t ownId, id_t parentId = id_t());

	NodeExclusionSet& exclusionSet();
	const NodeExclusionSet& exclusionSet() const;

	float3_t& point(std::size_t index);
	const float3_t& point(std::size_t index) const;
	PointList& points();
	const PointList& points() const;

	StaticProperty<float_t>& width();
	const StaticProperty<float_t>& width() const;

	StaticProperty<float_t>& orientation();
	const StaticProperty<float_t>& orientation() const;

	StaticProperty<bool>& killOnContact();
	const StaticProperty<bool>& killOnContact() const;

	AnimatedProperty<float_t>& bounce();
	const AnimatedProperty<float_t>& bounce() const;

	AnimatedProperty<float_t>& friction();
	const AnimatedProperty<float_t>& friction() const;

private:
	NodeExclusionSet colliderExclusionSet;
	PointList colliderPoints = PointList{
		float3_t(-0.5, 0.0, 0.0),
		float3_t(+0.5, 0.0, 0.0)
	};
	StaticProperty<float_t> colliderWidth = StaticProperty<float_t>(1.0);
	StaticProperty<float_t> colliderOrientation = StaticProperty<float_t>(0.0);
	StaticProperty<bool> colliderKillOnContact = StaticProperty<bool>(false);

	AnimatedProperty<float_t> colliderBounce = AnimatedProperty<float_t>(0.5);
	AnimatedProperty<float_t> colliderFriction = AnimatedProperty<float_t>(0.5);
};

void to_json(nlohmann::ordered_json& j, const Collider& collider);
void from_json(const nlohmann::ordered_json& j, Collider& collider);
}