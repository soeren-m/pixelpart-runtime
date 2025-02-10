#pragma once

#include "ForceField.h"
#include "AnimatedProperty.h"
#include "../common/Types.h"
#include "../common/Math.h"
#include "../json/json.hpp"
#include <vector>
#include <random>

namespace pixelpart {
class AccelerationField : public ForceField {
public:
	AccelerationField() = default;
	AccelerationField(id_t ownId, id_t parentId = id_t());

	virtual void applyInputs(const ComputeGraph::InputSet& inputs) override;

	virtual ForceFieldType forceFieldType() const override;

	AnimatedProperty<float3_t>& accelerationDirection();
	const AnimatedProperty<float3_t>& accelerationDirection() const;

	AnimatedProperty<float_t>& accelerationDirectionVariance();
	const AnimatedProperty<float_t>& accelerationDirectionVariance() const;

	AnimatedProperty<float_t>& accelerationStrengthVariance();
	const AnimatedProperty<float_t>& accelerationStrengthVariance() const;

	void accelerationGrid(int32_t x, int32_t y, int32_t z);
	void accelerationGrid(int32_t x, int32_t y, int32_t z,
		const std::vector<float3_t>& directionGrid, const std::vector<float_t>& strengthGrid);
	const std::vector<float3_t>& accelerationDirectionGrid() const;
	const std::vector<float_t>& accelerationStrengthGrid() const;
	int32_t accelerationGridSizeX() const;
	int32_t accelerationGridSizeY() const;
	int32_t accelerationGridSizeZ() const;

	template <typename T>
	void randomizeAccelerationGrid(T& rng) {
		std::uniform_real_distribution<float_t> uniformDistrib(-1.0, +1.0);
		fieldAccelerationDirectionGrid.resize(
			fieldAccelerationGridSize[0] * fieldAccelerationGridSize[1] * fieldAccelerationGridSize[2]);
		fieldAccelerationStrengthGrid.resize(
			fieldAccelerationGridSize[0] * fieldAccelerationGridSize[1] * fieldAccelerationGridSize[2]);

		for(float3_t& value : fieldAccelerationDirectionGrid) {
			value.x = uniformDistrib(rng);
			value.y = uniformDistrib(rng);
			value.z = uniformDistrib(rng);
		}
		for(float_t& value : fieldAccelerationStrengthGrid) {
			value = uniformDistrib(rng);
		}
	}

protected:
	virtual Node* cloneImpl() const override;

private:
	AnimatedProperty<float3_t> fieldAccelerationDirection = AnimatedProperty<float3_t>(float3_t(0.0));
	AnimatedProperty<float_t> fieldAccelerationDirectionVariance = AnimatedProperty<float_t>(0.0);
	AnimatedProperty<float_t> fieldAccelerationStrengthVariance = AnimatedProperty<float_t>(0.0);
	int32_t fieldAccelerationGridSize[3] = { 1, 1, 1 };
	std::vector<float3_t> fieldAccelerationDirectionGrid = std::vector<float3_t>{ float3_t(0.0) };
	std::vector<float_t> fieldAccelerationStrengthGrid = std::vector<float_t>{ 0.0 };
};

void to_json(nlohmann::ordered_json& j, const AccelerationField& field);
void from_json(const nlohmann::ordered_json& j, AccelerationField& field);
}