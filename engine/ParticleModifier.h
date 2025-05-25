#pragma once

#include "../common/Types.h"
#include "../effect/Effect.h"
#include "../effect/ParticleRuntimeId.h"
#include "ParticleRuntimeInstanceCollection.h"
#include "ParticleCollection.h"

namespace pixelpart {
class ParticleModifier {
public:
	virtual ~ParticleModifier() = default;

	virtual void run(const Effect* effect, RuntimeContext runtimeContext, ParticleRuntimeId runtimeId,
		ParticleCollection::WritePtr particles, std::uint32_t particleCount) const = 0;

	virtual void prepare(const Effect& effect, const RuntimeContext& runtimeContext) = 0;
};
}