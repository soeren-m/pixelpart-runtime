#pragma once

#include "Node.h"
#include "ForceFieldType.h"
#include "AnimatedProperty.h"
#include "../common/Types.h"
#include "../common/Id.h"
#include "../json/json.hpp"
#include <set>

namespace pixelpart {
class ForceField : public Node {
public:
	ForceField() = default;
	ForceField(id_t ownId, id_t parentId = id_t());

	virtual void applyInputs(const ComputeGraph::InputSet& inputs) override;

	virtual ForceFieldType forceFieldType() const = 0;

	void infinite(bool mode);
	bool infinite() const;

	std::set<id_t>& exclusionSet();
	const std::set<id_t>& exclusionSet() const;

	AnimatedProperty<float_t>& strength();
	const AnimatedProperty<float_t>& strength() const;

private:
	bool fieldInfinite = false;
	std::set<id_t> fieldExclusionSet;
	AnimatedProperty<float_t> fieldStrength = AnimatedProperty<float_t>(1.0);
};

void to_json(nlohmann::ordered_json& j, const ForceField& forceField);
void from_json(const nlohmann::ordered_json& j, ForceField& forceField);
}