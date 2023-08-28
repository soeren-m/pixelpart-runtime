#pragma once

#include "Node.h"
#include <random>

namespace pixelpart {
struct ForceField : public Node {
	enum class Type : uint32_t {
		point = 0,
		area = 1
	};

	Type type = Type::point;
	std::vector<uint32_t> exclusionList;
	Curve<vec3d> size = Curve<vec3d>(vec3d(1.0));
	Curve<vec3d> orientation = Curve<vec3d>(vec3d(0.0));

	Curve<vec3d> direction = Curve<vec3d>(vec3d(0.0));
	Curve<floatd> strength = Curve<floatd>(1.0);
	floatd directionVariance = 0.0;
	floatd strengthVariance = 0.0;

	uint32_t gridSize[3] = { 1, 1, 1 };
	std::vector<vec3d> directionGrid = std::vector<vec3d>{ vec3d(0.0) };
	std::vector<floatd> strengthGrid = std::vector<floatd>{ 0.0 };

	template <typename T>
	void randomize(T& rng) {
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

NLOHMANN_JSON_SERIALIZE_ENUM(ForceField::Type, {
	{ ForceField::Type::point, "point" },
	{ ForceField::Type::area, "area" }
})

void to_json(nlohmann::ordered_json& j, const ForceField& forceField);
void from_json(const nlohmann::ordered_json& j, ForceField& forceField);
}