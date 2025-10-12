#pragma once

#include "ParticleCollectionMap.h"
#include "ParticleEmissionStateMap.h"
#include "ParticleCollection.h"
#include "ParticleEmissionState.h"
#include "EffectRuntimeState.h"
#include "../common/Id.h"
#include "../effect/Effect.h"
#include "../effect/EffectRuntimeContext.h"
#include <cstdint>

namespace pixelpart {
class ParticleGenerator {
public:
	virtual ~ParticleGenerator() = default;

	virtual void generate(EffectRuntimeState& state,
		const Effect* effect, EffectRuntimeContext runtimeContext) const = 0;
	virtual void clear(EffectRuntimeState& state) const = 0;

	virtual void generate(EffectRuntimeState& state, std::uint32_t count,
		const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const = 0;
};
}
