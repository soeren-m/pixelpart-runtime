#pragma once

#include "ParticleModifier.h"
#include "SizeModifier.h"
#include "ColorModifier.h"
#include "AccelerationModifier.h"
#include "ForceModifier.h"
#include "CollisionModifier.h"
#include "MotionPathModifier.h"
#include "RotationModifier.h"
#include "IntegrationModifier.h"
#include "LifeModifier.h"
#include "Coordinate2dModifier.h"

namespace pixelpart {
class DefaultParticleModifier : public ParticleModifier {
public:
	DefaultParticleModifier() = default;

	virtual void apply(ParticleCollection::WritePtr particles, std::uint32_t particleCount,
		const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const override;

	virtual void reset(const Effect* effect, EffectRuntimeContext runtimeContext) override;

private:
	SizeModifier sizeModifier;
	ColorModifier colorModifier;
	AccelerationModifier accelerationModifier;
	ForceModifier forceModifier;
	CollisionModifier collisionModifier;
	MotionPathModifier motionPathModifier;
	RotationModifier rotationModifier;
	IntegrationModifier integrationModifier;
	LifeModifier lifeModifier;
	Coordinate2dModifier coordinate2dModifier;
};
}
