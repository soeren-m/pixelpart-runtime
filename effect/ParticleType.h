#pragma once

#include "RotationMode.h"
#include "AlignmentMode.h"
#include "MaterialInstance.h"
#include "ParticleRendererType.h"
#include "ParticleRendererSettings.h"
#include "StaticProperty.h"
#include "AnimatedProperty.h"
#include "ComputeGraph.h"
#include "../common/Types.h"
#include "../common/Math.h"
#include "../common/Id.h"
#include "../json/json.hpp"
#include <string>

namespace pixelpart {
class ParticleType {
public:
	friend class ParticleTypeCollection;

	ParticleType() = default;
	ParticleType(id_t ownId);

	void applyInputs(const ComputeGraph::InputSet& inputs);
	bool usesResource(const std::string& resourceId) const;

	id_t id() const;

	void parent(id_t parentId);
	id_t parentId() const;

	void name(const std::string& name);
	const std::string& name() const;

	void positionRelative(bool relative);
	bool positionRelative() const;

	void rotationMode(RotationMode mode);
	RotationMode rotationMode() const;

	void alignmentMode(AlignmentMode mode);
	AlignmentMode alignmentMode() const;

	StaticProperty<float3_t>& pivot();
	const StaticProperty<float3_t>& pivot() const;

	StaticProperty<float_t>& motionPathForce();
	const StaticProperty<float_t>& motionPathForce() const;

	void visible(bool mode);
	bool visible() const;

	void layer(std::uint32_t layer);
	std::uint32_t layer() const;

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

	AnimatedProperty<float_t>& count();
	const AnimatedProperty<float_t>& count() const;

	AnimatedProperty<float_t>& lifespan();
	const AnimatedProperty<float_t>& lifespan() const;

	AnimatedProperty<float_t>& initialSize();
	const AnimatedProperty<float_t>& initialSize() const;

	AnimatedProperty<float3_t>& initialRotation();
	const AnimatedProperty<float3_t>& initialRotation() const;

	AnimatedProperty<float_t>& initialVelocity();
	const AnimatedProperty<float_t>& initialVelocity() const;

	AnimatedProperty<float_t>& inheritedVelocity();
	const AnimatedProperty<float_t>& inheritedVelocity() const;

	AnimatedProperty<float_t>& initialOpacity();
	const AnimatedProperty<float_t>& initialOpacity() const;

	AnimatedProperty<float3_t>& motionPath();
	const AnimatedProperty<float3_t>& motionPath() const;

	AnimatedProperty<float3_t>& size();
	const AnimatedProperty<float3_t>& size() const;

	AnimatedProperty<float3_t>& stretch();
	const AnimatedProperty<float3_t>& stretch() const;

	AnimatedProperty<float_t>& physicalSize();
	const AnimatedProperty<float_t>& physicalSize() const;

	AnimatedProperty<float3_t>& rotation();
	const AnimatedProperty<float3_t>& rotation() const;

	AnimatedProperty<float3_t>& rotationBySpeed();
	const AnimatedProperty<float3_t>& rotationBySpeed() const;

	AnimatedProperty<float_t>& acceleration();
	const AnimatedProperty<float_t>& acceleration() const;

	AnimatedProperty<float_t>& radialAcceleration();
	const AnimatedProperty<float_t>& radialAcceleration() const;

	AnimatedProperty<float_t>& weight();
	const AnimatedProperty<float_t>& weight() const;

	AnimatedProperty<float_t>& bounce();
	const AnimatedProperty<float_t>& bounce() const;

	AnimatedProperty<float_t>& friction();
	const AnimatedProperty<float_t>& friction() const;

	AnimatedProperty<float4_t>& color();
	const AnimatedProperty<float4_t>& color() const;

	AnimatedProperty<float_t>& opacity();
	const AnimatedProperty<float_t>& opacity() const;

	StaticProperty<float_t>& lifespanVariance();
	const StaticProperty<float_t>& lifespanVariance() const;

	StaticProperty<float_t>& sizeVariance();
	const StaticProperty<float_t>& sizeVariance() const;

	StaticProperty<float3_t>& rotationVariance();
	const StaticProperty<float3_t>& rotationVariance() const;

	StaticProperty<float3_t>& angularVelocityVariance();
	const StaticProperty<float3_t>& angularVelocityVariance() const;

	StaticProperty<float_t>& velocityVariance();
	const StaticProperty<float_t>& velocityVariance() const;

	StaticProperty<float4_t>& colorVariance();
	const StaticProperty<float4_t>& colorVariance() const;

	StaticProperty<float_t>& opacityVariance();
	const StaticProperty<float_t>& opacityVariance() const;

private:
	id_t particleTypeId = id_t();
	id_t parentParticleTypeId = id_t();
	std::string particleTypeName;

	bool particlePositionRelative = false;
	RotationMode particleRotationMode = RotationMode::angle;
	AlignmentMode particleAlignmentMode = AlignmentMode::camera;

	StaticProperty<float3_t> particlePivot = StaticProperty<float3_t>(float3_t(0.0));
	StaticProperty<float_t> particleMotionPathForce = StaticProperty<float_t>(0.0);

	bool particleVisible = true;
	std::uint32_t particleLayer = 0;
	MaterialInstance particleMaterialInstance;
	ParticleRendererType particleRenderer = ParticleRendererType::sprite;
	ParticleSpriteRendererSettings particleSpriteRendererSettings;
	ParticleTrailRendererSettings particleTrailRendererSettings;
	ParticleMeshRendererSettings particleMeshRendererSettings;

	AnimatedProperty<float_t> particleCount = AnimatedProperty<float_t>(0.0);
	AnimatedProperty<float_t> particleLifespan = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> particleInitialSize = AnimatedProperty<float_t>(0.1);
	AnimatedProperty<float3_t> particleInitialRotation = AnimatedProperty<float3_t>(float3_t(0.0));
	AnimatedProperty<float_t> particleInitialVelocity = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> particleInheritedVelocity = AnimatedProperty<float_t>(0.0);
	AnimatedProperty<float_t> particleInitialOpacity = AnimatedProperty<float_t>(1.0);

	AnimatedProperty<float3_t> particleLifetimeMotionPath = AnimatedProperty<float3_t>(0.0, float3_t(0.0));
	AnimatedProperty<float3_t> particleLifetimeSize = AnimatedProperty<float3_t>(float3_t(1.0));
	AnimatedProperty<float3_t> particleLifetimeStretch = AnimatedProperty<float3_t>(float3_t(0.0));
	AnimatedProperty<float_t> particleLifetimePhysicalSize = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float3_t> particleLifetimeRotation = AnimatedProperty<float3_t>(float3_t(0.0));
	AnimatedProperty<float3_t> particleLifetimeRotationBySpeed = AnimatedProperty<float3_t>(float3_t(0.0));
	AnimatedProperty<float_t> particleLifetimeAcceleration = AnimatedProperty<float_t>(0.0);
	AnimatedProperty<float_t> particleLifetimeRadialAcceleration = AnimatedProperty<float_t>(0.0);
	AnimatedProperty<float_t> particleLifetimeWeight = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> particleLifetimeBounce = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float_t> particleLifetimeFriction = AnimatedProperty<float_t>(1.0);
	AnimatedProperty<float4_t> particleLifetimeColor = AnimatedProperty<float4_t>(float4_t(1.0));
	AnimatedProperty<float_t> particleLifetimeOpacity = AnimatedProperty<float_t>(1.0);

	StaticProperty<float_t> particleLifespanVariance = StaticProperty<float_t>(0.0);
	StaticProperty<float_t> particleSizeVariance = StaticProperty<float_t>(0.0);
	StaticProperty<float3_t> particleRotationVariance = StaticProperty<float3_t>(float3_t(0.0));
	StaticProperty<float3_t> particleAngularVelocityVariance = StaticProperty<float3_t>(float3_t(0.0));
	StaticProperty<float_t> particleVelocityVariance = StaticProperty<float_t>(0.0);
	StaticProperty<float4_t> particleColorVariance = StaticProperty<float4_t>(float4_t(0.0));
	StaticProperty<float_t> particleOpacityVariance = StaticProperty<float_t>(0.0);
};

void to_json(nlohmann::ordered_json& j, const ParticleType& particleType);
void from_json(const nlohmann::ordered_json& j, ParticleType& particleType);
}
