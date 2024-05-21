#pragma once

#include "Node.h"
#include "RotationMode.h"
#include "AlignmentMode.h"
#include "MaterialInstance.h"
#include "ParticleRendererType.h"
#include "ParticleRendererSettings.h"

namespace pixelpart {
struct ParticleType : public Node {
	AnimatedProperty<float_t> numParticles = AnimatedProperty<float_t>(0.0);
	AnimatedProperty<float_t> lifespan = AnimatedProperty<float_t>(1.0);
	StaticProperty<float_t> lifespanVariance = StaticProperty<float_t>(0.0);

	bool positionRelative = false;
	StaticProperty<float_t> motionPathForce = StaticProperty<float_t>(0.0);
	AnimatedProperty<float_t> initialVelocity = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> inheritedVelocity = AnimatedProperty<float_t>(0.0);
	StaticProperty<float_t> velocityVariance = StaticProperty<float_t>(0.0);
	AnimatedProperty<float_t> acceleration = AnimatedProperty<float_t>(0.0);
	AnimatedProperty<float_t> radialAcceleration = AnimatedProperty<float_t>(0.0);
	RotationMode rotationMode = RotationMode::angle;
	AlignmentMode alignmentMode = AlignmentMode::camera;
	AnimatedProperty<vec3_t> initialRotation = AnimatedProperty<vec3_t>(vec3_t(0.0));
	AnimatedProperty<vec3_t> rotation = AnimatedProperty<vec3_t>(vec3_t(0.0));
	AnimatedProperty<vec3_t> rotationBySpeed = AnimatedProperty<vec3_t>(vec3_t(0.0));
	StaticProperty<vec3_t> rotationVariance = StaticProperty<vec3_t>(vec3_t(0.0));
	StaticProperty<vec3_t> angularVelocityVariance = StaticProperty<vec3_t>(vec3_t(0.0));
	StaticProperty<vec3_t> pivot = StaticProperty<vec3_t>(vec3_t(0.0));

	AnimatedProperty<float_t> physicalSize = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> weight = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> bounce = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> friction = AnimatedProperty<float_t>(1.0);

	bool visible = true;
	uint32_t layer = 0u;
	AnimatedProperty<float_t> initialSize = AnimatedProperty<float_t>(0.1);
	AnimatedProperty<vec3_t> size = AnimatedProperty<vec3_t>(vec3_t(1.0));
	StaticProperty<float_t> sizeVariance = StaticProperty<float_t>(0.0);
	AnimatedProperty<vec3_t> stretch = AnimatedProperty<vec3_t>(vec3_t(0.0));
	AnimatedProperty<vec4_t> color = AnimatedProperty<vec4_t>(vec4_t(1.0));
	StaticProperty<vec4_t> colorVariance = StaticProperty<vec4_t>(vec4_t(0.0));
	AnimatedProperty<float_t> initialOpacity = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> opacity = AnimatedProperty<float_t>(1.0);
	StaticProperty<float_t> opacityVariance = StaticProperty<float_t>(0.0);

	MaterialInstance materialInstance;
	ParticleRendererType renderer = ParticleRendererType::sprite;
	ParticleSpriteRendererSettings spriteRendererSettings;
	ParticleTrailRendererSettings trailRendererSettings;
	ParticleMeshRendererSettings meshRendererSettings;
};

void to_json(nlohmann::ordered_json& j, const ParticleType& particleType);
void from_json(const nlohmann::ordered_json& j, ParticleType& particleType);
}