#pragma once

#include "Node.h"
#include "ColliderType.h"
#include "StaticProperty.h"
#include "AnimatedProperty.h"
#include "../common/Types.h"
#include "../common/Id.h"
#include "../json/json.hpp"
#include <set>

namespace pixelpart {
class Collider : public Node {
public:
	using PointList = std::vector<float3_t>;

	Collider() = default;
	Collider(id_t ownId, id_t parentId = id_t());

	virtual void applyInputs(const ComputeGraph::InputSet& inputs) override;

	virtual ColliderType colliderType() const = 0;

	std::set<id_t>& exclusionSet();
	const std::set<id_t>& exclusionSet() const;

	StaticProperty<bool>& killOnContact();
	const StaticProperty<bool>& killOnContact() const;

	AnimatedProperty<float_t>& bounce();
	const AnimatedProperty<float_t>& bounce() const;

	AnimatedProperty<float_t>& friction();
	const AnimatedProperty<float_t>& friction() const;

private:
	std::set<id_t> colliderExclusionSet;

	StaticProperty<bool> colliderKillOnContact = StaticProperty<bool>(false);

	AnimatedProperty<float_t> colliderBounce = AnimatedProperty<float_t>(0.5);
	AnimatedProperty<float_t> colliderFriction = AnimatedProperty<float_t>(0.5);
};

void to_json(nlohmann::ordered_json& j, const Collider& collider);
void from_json(const nlohmann::ordered_json& j, Collider& collider);
}
