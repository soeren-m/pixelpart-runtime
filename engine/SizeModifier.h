#pragma once

#include "ParticleModifier.h"

namespace pixelpart {
class SizeModifier : public ParticleModifier {
public:
	SizeModifier() = default;

	virtual void run(const SceneGraph& sceneGraph, const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		ParticleCollection::WritePtr particles, uint32_t particleCount, float_t t, float_t dt) const override;

	virtual void prepare(const Effect& effect, float_t t) override;
};
}