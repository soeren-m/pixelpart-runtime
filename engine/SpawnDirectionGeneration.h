#pragma once

#include "../effect/ParticleEmitter.h"
#include "../types/Types.h"
#include "../math/Pcg32.h"

namespace pixelpart {
float3_t generateParticleSpawnDirection(
	ParticleEmitter::DirectionMode directionMode,
	const float3_t& direction, float_t spread,
	const float3_t& parentDirection,
	const float3_t& particleSpawnPosition,
	bool effect3d,
	pcg32& rng);
}
