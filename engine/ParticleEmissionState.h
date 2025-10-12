#pragma once

#include "../common/Types.h"
#include <cstdint>

namespace pixelpart {
struct ParticleEmissionState {
	float_t emissionCount = 0.0;

	std::uint32_t emitterGridIndex = 0;
};
}
