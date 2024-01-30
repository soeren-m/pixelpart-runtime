#pragma once

#include "Node.h"
#include "BlendMode.h"
#include "RotationMode.h"
#include "AlignmentMode.h"
#include "MaterialInstance.h"
#include "ParticleRendererType.h"
#include "ParticleRendererSettings.h"

namespace pixelpart {
struct ParticleType : public Node {
	AnimatedProperty<floatd> numParticles = AnimatedProperty<floatd>(0.0);
	AnimatedProperty<floatd> lifespan = AnimatedProperty<floatd>(1.0);
	StaticProperty<floatd> lifespanVariance = StaticProperty<floatd>(0.0);

	bool positionRelative = false;
	StaticProperty<floatd> motionPathForce = StaticProperty<floatd>(0.0);
	AnimatedProperty<floatd> initialVelocity = AnimatedProperty<floatd>(1.0);
	AnimatedProperty<floatd> inheritedVelocity = AnimatedProperty<floatd>(0.0);
	StaticProperty<floatd> velocityVariance = StaticProperty<floatd>(0.0);
	AnimatedProperty<floatd> acceleration = AnimatedProperty<floatd>(0.0);
	AnimatedProperty<floatd> radialAcceleration = AnimatedProperty<floatd>(0.0);
	AnimatedProperty<floatd> damping = AnimatedProperty<floatd>(1.0);
	RotationMode rotationMode = RotationMode::angle;
	AlignmentMode alignmentMode = AlignmentMode::camera;
	AnimatedProperty<vec3d> initialRotation = AnimatedProperty<vec3d>(vec3d(0.0));
	AnimatedProperty<vec3d> rotation = AnimatedProperty<vec3d>(vec3d(0.0));
	AnimatedProperty<vec3d> rotationBySpeed = AnimatedProperty<vec3d>(vec3d(0.0));
	StaticProperty<vec3d> rotationVariance = StaticProperty<vec3d>(vec3d(0.0));
	StaticProperty<vec3d> angularVelocityVariance = StaticProperty<vec3d>(vec3d(0.0));
	StaticProperty<vec3d> pivot = StaticProperty<vec3d>(vec3d(0.0));

	AnimatedProperty<floatd> weight = AnimatedProperty<floatd>(1.0);
	AnimatedProperty<floatd> bounce = AnimatedProperty<floatd>(1.0);
	AnimatedProperty<floatd> friction = AnimatedProperty<floatd>(1.0);

	bool visible = true;
	uint32_t layer = 0;
	BlendMode blendMode = BlendMode::normal;
	AnimatedProperty<floatd> initialSize = AnimatedProperty<floatd>(0.1);
	AnimatedProperty<vec3d> size = AnimatedProperty<vec3d>(vec3d(1.0));
	StaticProperty<floatd> sizeVariance = StaticProperty<floatd>(0.0);
	AnimatedProperty<vec3d> stretch = AnimatedProperty<vec3d>(vec3d(0.0));
	AnimatedProperty<vec4d> color = AnimatedProperty<vec4d>(vec4d(1.0));
	StaticProperty<vec4d> colorVariance = StaticProperty<vec4d>(vec4d(0.0));
	AnimatedProperty<floatd> initialOpacity = AnimatedProperty<floatd>(1.0);
	AnimatedProperty<floatd> opacity = AnimatedProperty<floatd>(1.0);
	StaticProperty<floatd> opacityVariance = StaticProperty<floatd>(0.0);

	MaterialInstance materialInstance;
	ParticleRendererType renderer = ParticleRendererType::sprite;
	ParticleSpriteRendererSettings spriteRendererSettings;
	ParticleTrailRendererSettings trailRendererSettings;
	ParticleMeshRendererSettings meshRendererSettings;
};

void to_json(nlohmann::ordered_json& j, const ParticleType& particleType);
void from_json(const nlohmann::ordered_json& j, ParticleType& particleType);
}