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
class ParticleModifierPipeline : public ParticleModifier {
public:
	ParticleModifierPipeline() = default;

	virtual void run(const Effect* effect, RuntimeContext runtimeContext, ParticleRuntimeId runtimeId,
		ParticleCollection::WritePtr particles, std::uint32_t particleCount) const override;

	virtual void prepare(const Effect& effect, const RuntimeContext& runtimeContext) override;

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