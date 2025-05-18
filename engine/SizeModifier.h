#pragma once

#include "ParticleModifier.h"

namespace pixelpart {
class SizeModifier : public ParticleModifier {
public:
	SizeModifier() = default;

	virtual void run(const SceneGraph& sceneGraph, const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		ParticleCollection::WritePtr particles, std::uint32_t particleCount, const RuntimeContext& runtimeContext) const override;

	virtual void prepare(const Effect& effect, const RuntimeContext& runtimeContext) override;
};
}