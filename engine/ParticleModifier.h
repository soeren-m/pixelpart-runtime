#pragma once

#include "ParticleCollection.h"
#include "../effect/Effect.h"
#include "../effect/EffectRuntimeContext.h"
#include "../types/Types.h"
#include "../types/Id.h"
#include <cstdint>

namespace pixelpart {
class ParticleModifier {
public:
	virtual ~ParticleModifier() = default;

	virtual void apply(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
		const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const = 0;

	virtual void reset(const Effect* effect, EffectRuntimeContext runtimeContext) = 0;
};
}
