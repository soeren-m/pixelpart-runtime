#pragma once

#include "Node.h"
#include "Curve.h"
#include <bitset>
#include <random>

namespace pixelpart {
struct ForceField : public Node {
	enum class Type : uint32_t {
		point = 0, 
		area = 1
	};

	Type type = Type::point;
	std::bitset<256> emitterMask = std::bitset<256>().set();
	Curve<vec2d> motionPath = Curve<vec2d>(0.0, vec2d(0.0));
	Curve<floatd> width = Curve<floatd>(1.0);
	Curve<floatd> height = Curve<floatd>(1.0);
	Curve<floatd> orientation = Curve<floatd>(0.0);
	Curve<floatd> direction = Curve<floatd>(0.0);
	Curve<floatd> strength = Curve<floatd>(1.0);
	floatd directionVariance = 0.0;
	floatd strengthVariance = 0.0;

	uint32_t gridSize[2] = { 1, 1 };
	std::vector<floatd> directionGrid = std::vector<floatd>{ 0.0 };
	std::vector<floatd> strengthGrid = std::vector<floatd>{ 0.0 };

	template <typename T>
	void randomize(T& rng) {
		std::uniform_real_distribution<floatd> uniformDistrib(-1.0, +1.0);
		directionGrid.resize(gridSize[0] * gridSize[1]);
		strengthGrid.resize(gridSize[0] * gridSize[1]);

		for(floatd& value : directionGrid) {
			value = uniformDistrib(rng);
		}
		for(floatd& value : strengthGrid) {
			value = uniformDistrib(rng);
		}
	}
};

void to_json(nlohmann::ordered_json& j, const ForceField& forceField);
void from_json(const nlohmann::ordered_json& j, ForceField& forceField);
}