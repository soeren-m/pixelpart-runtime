#pragma once

#include "Node.h"
#include "BlendMode.h"
#include "RotationMode.h"
#include "AlignmentMode.h"
#include "ShaderGraph.h"
#include "ParticleRendererSettings.h"

namespace pixelpart {
struct ParticleType : public Node {
	enum class Renderer : uint32_t {
		sprite = 0,
		trail = 1
	};

	Curve<floatd> numParticles = Curve<floatd>(0.0);
	Curve<floatd> lifespan = Curve<floatd>(1.0);
	floatd lifespanVariance = 0.0;

	bool positionRelative = false;
	floatd motionPathForce = 0.0;
	Curve<floatd> initialVelocity = Curve<floatd>(1.0);
	floatd velocityVariance = 0.0;
	Curve<floatd> acceleration = Curve<floatd>(0.0);
	Curve<floatd> radialAcceleration = Curve<floatd>(0.0);
	Curve<floatd> damping = Curve<floatd>(1.0);
	RotationMode rotationMode = RotationMode::angle;
	AlignmentMode alignmentMode = AlignmentMode::camera;
	Curve<vec3d> initialRotation = Curve<vec3d>(vec3d(0.0));
	Curve<vec3d> rotation = Curve<vec3d>(vec3d(0.0));
	vec3d rotationVariance = vec3d(0.0);
	vec3d angularVelocityVariance = vec3d(0.0);
	vec3d pivot = vec3d(0.0);

	Curve<floatd> weight = Curve<floatd>(1.0);
	Curve<floatd> bounce = Curve<floatd>(1.0);
	Curve<floatd> friction = Curve<floatd>(1.0);

	bool visible = true;
	uint32_t layer = 0;
	BlendMode blendMode = BlendMode::normal;
	ShaderGraph shader;
	Curve<floatd> initialSize = Curve<floatd>(0.1);
	Curve<vec3d> size = Curve<vec3d>(vec3d(1.0));
	floatd sizeVariance = 0.0;
	Curve<vec4d> color = Curve<vec4d>(vec4d(1.0));
	vec4d colorVariance = vec4d(0.0);
	Curve<floatd> initialOpacity = Curve<floatd>(1.0);
	Curve<floatd> opacity = Curve<floatd>(1.0);
	floatd opacityVariance = 0.0;
	Renderer renderer = Renderer::sprite;
	ParticleSpriteRendererSettings spriteRendererSettings;
	ParticleTrailRendererSettings trailRendererSettings;
};

NLOHMANN_JSON_SERIALIZE_ENUM(ParticleType::Renderer, {
	{ ParticleType::Renderer::sprite, "sprite" },
	{ ParticleType::Renderer::trail, "trail" }
})

void to_json(nlohmann::ordered_json& j, const ParticleType& particleType);
void from_json(const nlohmann::ordered_json& j, ParticleType& particleType);
}