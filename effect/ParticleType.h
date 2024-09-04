#pragma once

#include "Node.h"
#include "../common/Types.h"
#include "../common/Math.h"
#include "RotationMode.h"
#include "AlignmentMode.h"
#include "MaterialInstance.h"
#include "ParticleRendererType.h"
#include "ParticleRendererSettings.h"
#include "StaticProperty.h"
#include "AnimatedProperty.h"
#include "../json/json.hpp"

namespace pixelpart {
class ParticleType : public Node {
public:
	ParticleType() = default;
	ParticleType(id_t ownId, id_t parentId = nullId);

	AnimatedProperty<float_t>& count();
	const AnimatedProperty<float_t>& count() const;

	AnimatedProperty<float_t>& lifespan();
	const AnimatedProperty<float_t>& lifespan() const;

	StaticProperty<float_t>& lifespanVariance();
	const StaticProperty<float_t>& lifespanVariance() const;

	void positionRelative(bool relative);
	bool positionRelative() const;

	StaticProperty<float_t>& motionPathForce();
	const StaticProperty<float_t>& motionPathForce() const;

	AnimatedProperty<float_t>& initialVelocity();
	const AnimatedProperty<float_t>& initialVelocity() const;

	AnimatedProperty<float_t>& inheritedVelocity();
	const AnimatedProperty<float_t>& inheritedVelocity() const;

	StaticProperty<float_t>& velocityVariance();
	const StaticProperty<float_t>& velocityVariance() const;

	AnimatedProperty<float_t>& acceleration();
	const AnimatedProperty<float_t>& acceleration() const;

	AnimatedProperty<float_t>& radialAcceleration();
	const AnimatedProperty<float_t>& radialAcceleration() const;

	void rotationMode(RotationMode mode);
	RotationMode rotationMode() const;

	void alignmentMode(AlignmentMode mode);
	AlignmentMode alignmentMode() const;

	AnimatedProperty<vec3_t>& initialRotation();
	const AnimatedProperty<vec3_t>& initialRotation() const;

	AnimatedProperty<vec3_t>& rotation();
	const AnimatedProperty<vec3_t>& rotation() const;

	AnimatedProperty<vec3_t>& rotationBySpeed();
	const AnimatedProperty<vec3_t>& rotationBySpeed() const;

	StaticProperty<vec3_t>& rotationVariance();
	const StaticProperty<vec3_t>& rotationVariance() const;

	StaticProperty<vec3_t>& angularVelocityVariance();
	const StaticProperty<vec3_t>& angularVelocityVariance() const;

	StaticProperty<vec3_t>& pivot();
	const StaticProperty<vec3_t>& pivot() const;

	AnimatedProperty<float_t>& physicalSize();
	const AnimatedProperty<float_t>& physicalSize() const;

	AnimatedProperty<float_t>& weight();
	const AnimatedProperty<float_t>& weight() const;

	AnimatedProperty<float_t>& bounce();
	const AnimatedProperty<float_t>& bounce() const;

	AnimatedProperty<float_t>& friction();
	const AnimatedProperty<float_t>& friction() const;

	void visible(bool mode);
	bool visible() const;

	void layer(uint32_t layer);
	uint32_t layer() const;

	AnimatedProperty<float_t>& initialSize();
	const AnimatedProperty<float_t>& initialSize() const;

	AnimatedProperty<vec3_t>& size();
	const AnimatedProperty<vec3_t>& size() const;

	StaticProperty<float_t>& sizeVariance();
	const StaticProperty<float_t>& sizeVariance() const;

	AnimatedProperty<vec3_t>& stretch();
	const AnimatedProperty<vec3_t>& stretch() const;

	AnimatedProperty<vec4_t>& color();
	const AnimatedProperty<vec4_t>& color() const;

	StaticProperty<vec4_t>& colorVariance();
	const StaticProperty<vec4_t>& colorVariance() const;

	AnimatedProperty<float_t>& initialOpacity();
	const AnimatedProperty<float_t>& initialOpacity() const;

	AnimatedProperty<float_t>& opacity();
	const AnimatedProperty<float_t>& opacity() const;

	StaticProperty<float_t>& opacityVariance();
	const StaticProperty<float_t>& opacityVariance() const;

	MaterialInstance& materialInstance();
	const MaterialInstance& materialInstance() const;

	void renderer(ParticleRendererType renderer);
	ParticleRendererType renderer() const;

	ParticleSpriteRendererSettings& spriteRendererSettings();
	const ParticleSpriteRendererSettings& spriteRendererSettings() const;

	ParticleTrailRendererSettings& trailRendererSettings();
	const ParticleTrailRendererSettings& trailRendererSettings() const;

	ParticleMeshRendererSettings& meshRendererSettings();
	const ParticleMeshRendererSettings& meshRendererSettings() const;

private:
	AnimatedProperty<float_t> particleCount = AnimatedProperty<float_t>(0.0);
	AnimatedProperty<float_t> particleLifespan = AnimatedProperty<float_t>(1.0);
	StaticProperty<float_t> particleLifespanVariance = StaticProperty<float_t>(0.0);

	bool particlePositionRelative = false;
	StaticProperty<float_t> particleMotionPathForce = StaticProperty<float_t>(0.0);
	AnimatedProperty<float_t> particleInitialVelocity = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> particleInheritedVelocity = AnimatedProperty<float_t>(0.0);
	StaticProperty<float_t> particleVelocityVariance = StaticProperty<float_t>(0.0);
	AnimatedProperty<float_t> particleAcceleration = AnimatedProperty<float_t>(0.0);
	AnimatedProperty<float_t> particleRadialAcceleration = AnimatedProperty<float_t>(0.0);
	RotationMode particleRotationMode = RotationMode::angle;
	AlignmentMode particleAlignmentMode = AlignmentMode::camera;
	AnimatedProperty<vec3_t> particleInitialRotation = AnimatedProperty<vec3_t>(vec3_t(0.0));
	AnimatedProperty<vec3_t> particleRotation = AnimatedProperty<vec3_t>(vec3_t(0.0));
	AnimatedProperty<vec3_t> particleRotationBySpeed = AnimatedProperty<vec3_t>(vec3_t(0.0));
	StaticProperty<vec3_t> particleRotationVariance = StaticProperty<vec3_t>(vec3_t(0.0));
	StaticProperty<vec3_t> particleAngularVelocityVariance = StaticProperty<vec3_t>(vec3_t(0.0));
	StaticProperty<vec3_t> particlePivot = StaticProperty<vec3_t>(vec3_t(0.0));

	AnimatedProperty<float_t> particlePhysicalSize = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> particleWeight = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> particleBounce = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> particleFriction = AnimatedProperty<float_t>(1.0);

	bool particleVisible = true;
	uint32_t particleLayer = 0u;
	AnimatedProperty<float_t> particleInitialSize = AnimatedProperty<float_t>(0.1);
	AnimatedProperty<vec3_t> particleSize = AnimatedProperty<vec3_t>(vec3_t(1.0));
	StaticProperty<float_t> particleSizeVariance = StaticProperty<float_t>(0.0);
	AnimatedProperty<vec3_t> particleStretch = AnimatedProperty<vec3_t>(vec3_t(0.0));
	AnimatedProperty<vec4_t> particleColor = AnimatedProperty<vec4_t>(vec4_t(1.0));
	StaticProperty<vec4_t> particleColorVariance = StaticProperty<vec4_t>(vec4_t(0.0));
	AnimatedProperty<float_t> particleInitialOpacity = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> particleOpacity = AnimatedProperty<float_t>(1.0);
	StaticProperty<float_t> particleOpacityVariance = StaticProperty<float_t>(0.0);
	MaterialInstance particleMaterialInstance;
	ParticleRendererType particleRenderer = ParticleRendererType::sprite;
	ParticleSpriteRendererSettings particleSpriteRendererSettings;
	ParticleTrailRendererSettings particleTrailRendererSettings;
	ParticleMeshRendererSettings particleMeshRendererSettings;
};

void to_json(nlohmann::ordered_json& j, const ParticleType& particleType);
void from_json(const nlohmann::ordered_json& j, ParticleType& particleType);
}