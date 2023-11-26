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
		Curve<vec3d> direction = Curve<vec3d>(vec3d(0.0));
		floatd directionVariance = 0.0;
		floatd strengthVariance = 0.0;

		int32_t gridSize[3] = { 1, 1, 1 };
		std::vector<vec3d> directionGrid = std::vector<vec3d>{ vec3d(0.0) };
		std::vector<floatd> strengthGrid = std::vector<floatd>{ 0.0 };

		template <typename T>
		void randomizeGrid(T& rng) {
			std::uniform_real_distribution<floatd> uniformDistrib(-1.0, +1.0);
			directionGrid.resize(gridSize[0] * gridSize[1] * gridSize[2]);
			strengthGrid.resize(gridSize[0] * gridSize[1] * gridSize[2]);

			for(vec3d& value : directionGrid) {
				value.x = uniformDistrib(rng);
				value.y = uniformDistrib(rng);
				value.z = uniformDistrib(rng);
			}
			for(floatd& value : strengthGrid) {
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

		Curve<floatd> tightness = Curve<floatd>(1.0);
	};

	struct NoiseField {
		uint32_t octaves = 4u;
		Curve<floatd> frequency = Curve<floatd>(1.0);
		Curve<floatd> persistence = Curve<floatd>(0.5);
		Curve<floatd> lacunarity = Curve<floatd>(2.0);

		bool animated = false;
		floatd animationTimeScale = 1.0;
		floatd animationTimeBase = 0.0;
	};

	struct DragField {
		floatd velocityInfluence = 1.0;
		floatd sizeInfluence = 1.0;
	};

	Type type = Type::attraction_field;
	std::vector<uint32_t> exclusionList;
	Curve<vec3d> size = Curve<vec3d>(vec3d(1.0));
	Curve<vec3d> orientation = Curve<vec3d>(vec3d(0.0));

	Curve<floatd> strength = Curve<floatd>(1.0);

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