#pragma once

#include "Node.h"
#include "../common/Types.h"
#include "../common/Math.h"
#include "../common/Id.h"
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
	ParticleType(id_t ownId, id_t parentId = id_t());

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

	AnimatedProperty<float3_t>& initialRotation();
	const AnimatedProperty<float3_t>& initialRotation() const;

	AnimatedProperty<float3_t>& rotation();
	const AnimatedProperty<float3_t>& rotation() const;

	AnimatedProperty<float3_t>& rotationBySpeed();
	const AnimatedProperty<float3_t>& rotationBySpeed() const;

	StaticProperty<float3_t>& rotationVariance();
	const StaticProperty<float3_t>& rotationVariance() const;

	StaticProperty<float3_t>& angularVelocityVariance();
	const StaticProperty<float3_t>& angularVelocityVariance() const;

	StaticProperty<float3_t>& pivot();
	const StaticProperty<float3_t>& pivot() const;

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

	AnimatedProperty<float3_t>& size();
	const AnimatedProperty<float3_t>& size() const;

	StaticProperty<float_t>& sizeVariance();
	const StaticProperty<float_t>& sizeVariance() const;

	AnimatedProperty<float3_t>& stretch();
	const AnimatedProperty<float3_t>& stretch() const;

	AnimatedProperty<float4_t>& color();
	const AnimatedProperty<float4_t>& color() const;

	StaticProperty<float4_t>& colorVariance();
	const StaticProperty<float4_t>& colorVariance() const;

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
	AnimatedProperty<float3_t> particleInitialRotation = AnimatedProperty<float3_t>(float3_t(0.0));
	AnimatedProperty<float3_t> particleRotation = AnimatedProperty<float3_t>(float3_t(0.0));
	AnimatedProperty<float3_t> particleRotationBySpeed = AnimatedProperty<float3_t>(float3_t(0.0));
	StaticProperty<float3_t> particleRotationVariance = StaticProperty<float3_t>(float3_t(0.0));
	StaticProperty<float3_t> particleAngularVelocityVariance = StaticProperty<float3_t>(float3_t(0.0));
	StaticProperty<float3_t> particlePivot = StaticProperty<float3_t>(float3_t(0.0));

	AnimatedProperty<float_t> particlePhysicalSize = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> particleWeight = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> particleBounce = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> particleFriction = AnimatedProperty<float_t>(1.0);

	bool particleVisible = true;
	uint32_t particleLayer = 0u;
	AnimatedProperty<float_t> particleInitialSize = AnimatedProperty<float_t>(0.1);
	AnimatedProperty<float3_t> particleSize = AnimatedProperty<float3_t>(float3_t(1.0));
	StaticProperty<float_t> particleSizeVariance = StaticProperty<float_t>(0.0);
	AnimatedProperty<float3_t> particleStretch = AnimatedProperty<float3_t>(float3_t(0.0));
	AnimatedProperty<float4_t> particleColor = AnimatedProperty<float4_t>(float4_t(1.0));
	StaticProperty<float4_t> particleColorVariance = StaticProperty<float4_t>(float4_t(0.0));
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