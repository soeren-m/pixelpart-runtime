#pragma once

#include "ParticleModifier.h"

namespace pixelpart {
class Coordinate2dModifier : public ParticleModifier {
public:
	Coordinate2dModifier() = default;

	virtual void run(const SceneGraph& sceneGraph, const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		ParticleCollection::WritePtr particles, uint32_t particleCount, const RuntimeContext& runtimeContext) const override;

	virtual void prepare(const Effect& effect, const RuntimeContext& runtimeContext) override;

private:
	bool effect3d = false;
};
}