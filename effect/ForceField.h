#pragma once

#include "Node.h"
#include "../common/Types.h"
#include "../common/Math.h"
#include "NodeExclusionSet.h"
#include "StaticProperty.h"
#include "AnimatedProperty.h"
#include "../json/json.hpp"
#include <random>
#include <string>
#include <vector>

namespace pixelpart {
class ForceField : public Node {
public:
	enum class Type : uint32_t {
		attraction_field = 0,
		acceleration_field = 1,
		vector_field = 2,
		noise_field = 3,
		drag_field = 4
	};
	enum class Filter : uint32_t {
		none = 0,
		linear = 1
	};

	ForceField() = default;
	ForceField(id_t ownId, id_t parentId = nullId);

	void type(Type type);
	Type type() const;

	NodeExclusionSet& exclusionSet();
	const NodeExclusionSet& exclusionSet() const;

	AnimatedProperty<vec3_t>& size();
	const AnimatedProperty<vec3_t>& size() const;

	AnimatedProperty<vec3_t>& orientation();
	const AnimatedProperty<vec3_t>& orientation() const;

	AnimatedProperty<float_t>& strength();
	const AnimatedProperty<float_t>& strength() const;

	AnimatedProperty<vec3_t>& accelerationDirection();
	const AnimatedProperty<vec3_t>& accelerationDirection() const;

	AnimatedProperty<float_t>& accelerationDirectionVariance();
	const AnimatedProperty<float_t>& accelerationDirectionVariance() const;

	AnimatedProperty<float_t>& accelerationStrengthVariance();
	const AnimatedProperty<float_t>& accelerationStrengthVariance() const;

	void accelerationGridSize(int32_t x, int32_t y, int32_t z);
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

		for(vec3_t& value : fieldAccelerationDirectionGrid) {
			value.x = uniformDistrib(rng);
			value.y = uniformDistrib(rng);
			value.z = uniformDistrib(rng);
		}
		for(float_t& value : fieldAccelerationStrengthGrid) {
			value = uniformDistrib(rng);
		}
	}

	void vectorResourceId(const std::string& resourceId);
	const std::string& vectorResourceId() const;

	void vectorFilter(Filter filter);
	Filter vectorFilter() const;

	AnimatedProperty<float_t>& vectorTightness();
	const AnimatedProperty<float_t>& vectorTightness() const;

	// TODO:

	StaticProperty<float_t>& dragVelocityInfluence();
	const StaticProperty<float_t>& dragVelocityInfluence() const;

	StaticProperty<float_t>& dragSizeInfluence();
	const StaticProperty<float_t>& dragSizeInfluence() const;

private:
	Type fieldType = Type::attraction_field;
	NodeExclusionSet fieldExclusionSet;
	AnimatedProperty<vec3_t> fieldSize = AnimatedProperty<vec3_t>(vec3_t(1.0));
	AnimatedProperty<vec3_t> fieldOrientation = AnimatedProperty<vec3_t>(vec3_t(0.0));
	AnimatedProperty<float_t> fieldStrength = AnimatedProperty<float_t>(1.0);

	AnimatedProperty<vec3_t> fieldAccelerationDirection = AnimatedProperty<vec3_t>(vec3_t(0.0));
	AnimatedProperty<float_t> fieldAccelerationDirectionVariance = AnimatedProperty<float_t>(0.0);
	AnimatedProperty<float_t> fieldAccelerationStrengthVariance = AnimatedProperty<float_t>(0.0);
	int32_t fieldAccelerationGridSize[3] = { 1, 1, 1 };
	std::vector<vec3_t> fieldAccelerationDirectionGrid = std::vector<vec3_t>{ vec3_t(0.0) };
	std::vector<float_t> fieldAccelerationStrengthGrid = std::vector<float_t>{ 0.0 };

	std::string fieldVectorResourceId;
	Filter fieldVectorFilter = Filter::none;
	AnimatedProperty<float_t> fieldVectorTightness = AnimatedProperty<float_t>(1.0);

	StaticProperty<int64_t> fieldNoiseOctaves = StaticProperty<int64_t>(4);
	AnimatedProperty<float_t> fieldNoiseFrequency = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> fieldNoisePersistence = AnimatedProperty<float_t>(0.5);
	AnimatedProperty<float_t> fieldNoiseLacunarity = AnimatedProperty<float_t>(2.0);
	bool fieldNoiseAnimated = false;
	StaticProperty<float_t> fieldNoiseAnimationTimeScale = StaticProperty<float_t>(1.0);
	StaticProperty<float_t> fieldNoiseAnimationTimeBase = StaticProperty<float_t>(0.0);

	StaticProperty<float_t> fieldDragVelocityInfluence = StaticProperty<float_t>(1.0);
	StaticProperty<float_t> fieldDragSizeInfluence = StaticProperty<float_t>(1.0);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ForceField::Type, {
	{ ForceField::Type::attraction_field, "attraction_field" },
	{ ForceField::Type::acceleration_field, "acceleration_field" },
	{ ForceField::Type::vector_field, "vector_field" },
	{ ForceField::Type::noise_field, "noise_field" },
	{ ForceField::Type::drag_field, "drag_field" }
})

NLOHMANN_JSON_SERIALIZE_ENUM(ForceField::Filter, {
	{ ForceField::Filter::none, "none" },
	{ ForceField::Filter::linear, "linear" }
})

void to_json(nlohmann::ordered_json& j, const ForceField& forceField);
void from_json(const nlohmann::ordered_json& j, ForceField& forceField);
}