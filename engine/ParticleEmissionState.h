#pragma once

#include "../types/Types.h"
#include "../effect/Transform.h"
#include <cstdint>

namespace pixelpart {
struct ParticleEmissionState {
	float_t emissionCount = 0.0;

	Transform emitterPrevTransform;
	std::uint32_t emitterGridIndex = 0;
};
}
