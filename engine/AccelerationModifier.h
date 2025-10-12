#pragma once

#include "ParticleModifier.h"

namespace pixelpart {
class AccelerationModifier : public ParticleModifier {
public:
	AccelerationModifier() = default;

	virtual void apply(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
		const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const override;

	virtual void reset(const Effect* effect, EffectRuntimeContext runtimeContext) override;
};
}
