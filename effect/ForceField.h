#pragma once

#include "Node.h"
#include <random>

namespace pixelpart {
struct ForceField : public Node {
	enum class Type : uint32_t {
		attraction_field = 0,
		acceleration_field = 1,
		vector_field = 2,
		noise_field = 3,
		drag_field = 4
	};

	struct AttractionField {

	};

	struct AccelerationField {
		AnimatedProperty<vec3_t> direction = AnimatedProperty<vec3_t>(vec3_t(0.0));
		AnimatedProperty<float_t> directionVariance = AnimatedProperty<float_t>(0.0);
		AnimatedProperty<float_t> strengthVariance = AnimatedProperty<float_t>(0.0);

		int32_t gridSize[3] = { 1, 1, 1 };
		std::vector<vec3_t> directionGrid = std::vector<vec3_t>{ vec3_t(0.0) };
		std::vector<float_t> strengthGrid = std::vector<float_t>{ 0.0 };

		template <typename T>
		void randomizeGrid(T& rng) {
			std::uniform_real_distribution<float_t> uniformDistrib(-1.0, +1.0);
			directionGrid.resize(gridSize[0] * gridSize[1] * gridSize[2]);
			strengthGrid.resize(gridSize[0] * gridSize[1] * gridSize[2]);

			for(vec3_t& value : directionGrid) {
				value.x = uniformDistrib(rng);
				value.y = uniformDistrib(rng);
				value.z = uniformDistrib(rng);
			}
			for(float_t& value : strengthGrid) {
				value = uniformDistrib(rng);
			}
		}
	};

	struct VectorField {
		enum class Filter : uint32_t {
			none = 0,
			linear = 1
		};

		std::string resourceId;
		Filter filter = Filter::none;

		AnimatedProperty<float_t> tightness = AnimatedProperty<float_t>(1.0);
	};

	struct NoiseField {
		StaticProperty<int64_t> octaves = StaticProperty<int64_t>(4);
		AnimatedProperty<float_t> frequency = AnimatedProperty<float_t>(1.0);
		AnimatedProperty<float_t> persistence = AnimatedProperty<float_t>(0.5);
		AnimatedProperty<float_t> lacunarity = AnimatedProperty<float_t>(2.0);

		bool animated = false;
		StaticProperty<float_t> animationTimeScale = StaticProperty<float_t>(1.0);
		StaticProperty<float_t> animationTimeBase = StaticProperty<float_t>(0.0);
	};

	struct DragField {
		StaticProperty<float_t> velocityInfluence = StaticProperty<float_t>(1.0);
		StaticProperty<float_t> sizeInfluence = StaticProperty<float_t>(1.0);
	};

	Type type = Type::attraction_field;
	std::vector<id_t> exclusionList;
	AnimatedProperty<vec3_t> size = AnimatedProperty<vec3_t>(vec3_t(1.0));
	AnimatedProperty<vec3_t> orientation = AnimatedProperty<vec3_t>(vec3_t(0.0));

	AnimatedProperty<float_t> strength = AnimatedProperty<float_t>(1.0);

	AttractionField attractionField;
	AccelerationField accelerationField;
	VectorField vectorField;
	NoiseField noiseField;
	DragField dragField;
};

NLOHMANN_JSON_SERIALIZE_ENUM(ForceField::Type, {
	{ ForceField::Type::attraction_field, "attraction_field" },
	{ ForceField::Type::acceleration_field, "acceleration_field" },
	{ ForceField::Type::vector_field, "vector_field" },
	{ ForceField::Type::noise_field, "noise_field" },
	{ ForceField::Type::drag_field, "drag_field" }
})

NLOHMANN_JSON_SERIALIZE_ENUM(ForceField::VectorField::Filter, {
	{ ForceField::VectorField::Filter::none, "none" },
	{ ForceField::VectorField::Filter::linear, "linear" }
})

void to_json(nlohmann::ordered_json& j, const ForceField::AttractionField& field);
void to_json(nlohmann::ordered_json& j, const ForceField::AccelerationField& field);
void to_json(nlohmann::ordered_json& j, const ForceField::VectorField& field);
void to_json(nlohmann::ordered_json& j, const ForceField::NoiseField& field);
void to_json(nlohmann::ordered_json& j, const ForceField::DragField& field);
void to_json(nlohmann::ordered_json& j, const ForceField& forceField);
void from_json(const nlohmann::ordered_json& j, ForceField::AttractionField& field);
void from_json(const nlohmann::ordered_json& j, ForceField::AccelerationField& field);
void from_json(const nlohmann::ordered_json& j, ForceField::VectorField& field);
void from_json(const nlohmann::ordered_json& j, ForceField::NoiseField& field);
void from_json(const nlohmann::ordered_json& j, ForceField::DragField& field);
void from_json(const nlohmann::ordered_json& j, ForceField& forceField);
}