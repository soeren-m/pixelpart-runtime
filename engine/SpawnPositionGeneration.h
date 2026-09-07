#pragma once

#include "../effect/ParticleEmitter.h"
#include "../effect/Polyline.h"
#include "../math/Pcg32.h"
#include "../types/Types.h"
#include <cstdint>

namespace pixelpart {
float3_t generateParticleSpawnPosition(
	const float3_t& scale,
	ParticleEmitter::Shape shape,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	const std::uint32_t gridSize[],
	const Polyline& path,
	std::uint32_t& gridIndex,
	pcg32& rng);
}
