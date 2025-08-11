#pragma once

#include "ParticleModifier.h"

namespace pixelpart {
class MotionPathModifier : public ParticleModifier {
public:
	MotionPathModifier() = default;

	virtual void run(const Effect* effect, RuntimeContext runtimeContext, ParticleRuntimeId runtimeId,
		ParticleCollection::WritePtr particles, std::uint32_t particleCount) const override;

	virtual void prepare(const Effect& effect, const RuntimeContext& runtimeContext) override;
};
}
