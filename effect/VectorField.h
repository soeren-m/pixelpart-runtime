#pragma once

#include "ForceField.h"
#include "AnimatedProperty.h"
#include "../common/Types.h"
#include "../json/json.hpp"
#include <string>

namespace pixelpart {
class VectorField : public ForceField {
public:
	enum class Filter : std::uint32_t {
		none = 0,
		linear = 1
	};

	VectorField() = default;
	VectorField(id_t ownId, id_t parentId = id_t());

	virtual void applyInputs(const ComputeGraph::InputSet& inputs) override;
	virtual bool usesResource(const std::string& resourceId) const override;

	virtual ForceFieldType forceFieldType() const override;

	void vectorFieldResourceId(const std::string& resourceId);
	const std::string& vectorFieldResourceId() const;

	void vectorFieldFilter(Filter filter);
	Filter vectorFieldFilter() const;

	AnimatedProperty<float_t>& tightness();
	const AnimatedProperty<float_t>& tightness() const;

protected:
	virtual Node* cloneImpl() const override;

private:
	std::string fieldVectorFieldResourceId;
	Filter fieldVectorFieldFilter = Filter::none;
	AnimatedProperty<float_t> fieldTightness = AnimatedProperty<float_t>(1.0);
};

NLOHMANN_JSON_SERIALIZE_ENUM(VectorField::Filter, {
	{ VectorField::Filter::none, "none" },
	{ VectorField::Filter::linear, "linear" }
})

void to_json(nlohmann::ordered_json& j, const VectorField& field);
void from_json(const nlohmann::ordered_json& j, VectorField& field);
}