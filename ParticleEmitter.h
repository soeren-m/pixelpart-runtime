#pragma once

#include "Node.h"
#include "Curve.h"
#include "ImageReference.h"
#include "ImageAnimation.h"
#include "ColorMode.h"
#include "BlendMode.h"

namespace pixelpart {
struct ParticleEmitter : public Node {
	enum class Shape : uint32_t {
		point = 0,
		line = 1,
		ellipse = 2,
		rectangle = 3
	};
	enum class Distribution : uint32_t {
		uniform = 0,
		center = 1,
		center_reverse = 2,
		boundary = 3
	};
	enum class SpawnMode : uint32_t {
		fixed = 0,
		out = 1,
		in = 2
	};
	enum class RotationMode : uint32_t {
		angle = 0,
		velocity = 1,
		align = 2,
		alignpath = 3
	};
	enum class RendererType : uint32_t {
		sprite = 0,
		trail = 1
	};

	uint32_t id = NullId;
	uint32_t parentId = NullId;

	Shape shape = Shape::point;
	Distribution distribution = Distribution::uniform;
	SpawnMode spawnMode = SpawnMode::fixed;
	Curve<floatd> width = Curve<floatd>(0.0);
	Curve<floatd> height = Curve<floatd>(0.0);
	Curve<floatd> orientation = Curve<floatd>(0.0);
	Curve<floatd> direction = Curve<floatd>(0.0);
	Curve<floatd> spread = Curve<floatd>(0.0);
	Curve<floatd> numParticles = Curve<floatd>(0.0);
	bool burst = false;
	Curve<floatd> particleLifespan = Curve<floatd>(1.0);
	floatd particleLifespanVariance = 0.0;
	uint32_t particleSpawnOnStepEmitterId = NullId;
	floatd particleSpawnOnStepProb = 0.1;
	uint32_t particleSpawnOnDeathEmitterId = NullId;
	uint32_t particleSpawnOnDeathNumber = 1.0;
	floatd particleSpawnOnDeathProb = 1.0;
	
	Curve<vec2d> motionPath = Curve<vec2d>(0.0, vec2d(0.0));
	Curve<vec2d> particleMotionPath = Curve<vec2d>();
	floatd particleMotionPathInfluence = 10.0;
	bool particlePositionRelative = false;
	Curve<floatd> particleInitialVelocity = Curve<floatd>(1.0);
	floatd particleInitialVelocityVariance = 0.0;
	Curve<floatd> particleAcceleration = Curve<floatd>(0.0);
	Curve<floatd> particleRadialAcceleration = Curve<floatd>(0.0);
	Curve<floatd> particleDamping = Curve<floatd>(1.0);
	Curve<floatd> particleInitialRotation = Curve<floatd>(0.0);
	Curve<floatd> particleRotation = Curve<floatd>(0.0);
	RotationMode particleRotationMode = RotationMode::angle;
	floatd particleRotationVariance = 0.0;
	floatd particleAngularVelocityVariance = 0.0;

	Curve<floatd> particleWeight = Curve<floatd>(1.0);
	Curve<floatd> particleBounce = Curve<floatd>(1.0);
	Curve<floatd> particleFriction = Curve<floatd>(1.0);
	
	ImageReference particleSprite;
	ImageAnimation particleSpriteAnimation;
	Curve<floatd> particleInitialSize = Curve<floatd>(1.0);
	Curve<floatd> particleSize = Curve<floatd>(0.1);
	Curve<floatd> particleWidth = Curve<floatd>(1.0);
	Curve<floatd> particleHeight = Curve<floatd>(1.0);
	floatd particleSizeVariance = 0.0;
	Curve<vec4d> particleColor = Curve<vec4d>(vec4d(1.0));
	Curve<floatd> particleInitialOpacity = Curve<floatd>(1.0);
	Curve<floatd> particleOpacity = Curve<floatd>(1.0);
	vec4d particleColorVariance = vec4d(0);
	floatd particleOpacityVariance = 0.0;
	floatd alphaThreshold = 0.0;
	BlendMode blendMode = BlendMode::normal;
	ColorMode colorMode = ColorMode::multiply;
	RendererType renderer = RendererType::sprite;
	floatd rendererTrailUVFactor = 1.0;
	uint32_t rendererTrailUVRotation = 0;
	uint32_t rendererNumTrailSegments = 0;
	uint32_t layer = 0;
	bool visible = true;
};

void to_json(nlohmann::ordered_json& j, const ParticleEmitter& emitter);
void from_json(const nlohmann::ordered_json& j, ParticleEmitter& emitter);
}