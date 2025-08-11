#pragma once

#include "ForceField.h"
#include "StaticProperty.h"
#include "AnimatedProperty.h"
#include "../common/Types.h"
#include "../json/json.hpp"

namespace pixelpart {
class NoiseField : public ForceField {
public:
	NoiseField() = default;
	NoiseField(id_t ownId, id_t parentId = id_t());

	virtual void applyInputs(const ComputeGraph::InputSet& inputs) override;

	virtual ForceFieldType forceFieldType() const override;

	StaticProperty<int_t>& noiseOctaves();
	const StaticProperty<int_t>& noiseOctaves() const;

	AnimatedProperty<float_t>& noiseFrequency();
	const AnimatedProperty<float_t>& noiseFrequency() const;

	AnimatedProperty<float_t>& noisePersistence();
	const AnimatedProperty<float_t>& noisePersistence() const;

	AnimatedProperty<float_t>& noiseLacunarity();
	const AnimatedProperty<float_t>& noiseLacunarity() const;

	void noiseAnimated(bool enable);
	bool noiseAnimated() const;

	StaticProperty<float_t>& noiseAnimationTimeScale();
	const StaticProperty<float_t>& noiseAnimationTimeScale() const;

	StaticProperty<float_t>& noiseAnimationTimeBase();
	const StaticProperty<float_t>& noiseAnimationTimeBase() const;

protected:
	virtual Node* cloneImpl() const override;

private:
	StaticProperty<int_t> fieldNoiseOctaves = StaticProperty<int_t>(4);
	AnimatedProperty<float_t> fieldNoiseFrequency = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> fieldNoisePersistence = AnimatedProperty<float_t>(0.5);
	AnimatedProperty<float_t> fieldNoiseLacunarity = AnimatedProperty<float_t>(2.0);
	bool fieldNoiseAnimated = false;
	StaticProperty<float_t> fieldNoiseAnimationTimeScale = StaticProperty<float_t>(1.0);
	StaticProperty<float_t> fieldNoiseAnimationTimeBase = StaticProperty<float_t>(0.0);
};

void to_json(nlohmann::ordered_json& j, const NoiseField& field);
void from_json(const nlohmann::ordered_json& j, NoiseField& field);
}
