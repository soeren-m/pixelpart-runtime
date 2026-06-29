#pragma once

#include "../types/Types.h"
#include "../json/json.hpp"
#include <cstdint>

namespace pixelpart {
class ParticleLodStrategy {
public:
	ParticleLodStrategy() = default;

	void hide(bool hide);
	bool hide() const;

	void emissionFactor(float_t factor);
	float_t emissionFactor() const;

	void lifetimeFactor(float_t factor);
	float_t lifetimeFactor() const;

private:
	bool strategyHide = false;
	float_t strategyEmissionFactor = 1.0;
	float_t strategyLifetimeFactor = 1.0;
};

void to_json(nlohmann::ordered_json& j, const ParticleLodStrategy& lodStrategy);
void from_json(const nlohmann::ordered_json& j, ParticleLodStrategy& lodStrategy);
}
