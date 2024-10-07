#include "ParticleType.h"

namespace pixelpart {
ParticleType::ParticleType(id_t ownId, id_t parentId) : Node(ownId, parentId) {

}

AnimatedProperty<float_t>& ParticleType::count() {
	return particleCount;
}
const AnimatedProperty<float_t>& ParticleType::count() const {
	return particleCount;
}

AnimatedProperty<float_t>& ParticleType::lifespan() {
	return particleLifespan;
}
const AnimatedProperty<float_t>& ParticleType::lifespan() const {
	return particleLifespan;
}

StaticProperty<float_t>& ParticleType::lifespanVariance() {
	return particleLifespanVariance;
}
const StaticProperty<float_t>& ParticleType::lifespanVariance() const {
	return particleLifespanVariance;
}

void ParticleType::positionRelative(bool relative) {
	particlePositionRelative = relative;
}
bool ParticleType::positionRelative() const {
	return particlePositionRelative;
}

StaticProperty<float_t>& ParticleType::motionPathForce() {
	return particleMotionPathForce;
}
const StaticProperty<float_t>& ParticleType::motionPathForce() const {
	return particleMotionPathForce;
}

AnimatedProperty<float_t>& ParticleType::initialVelocity() {
	return particleInitialVelocity;
}
const AnimatedProperty<float_t>& ParticleType::initialVelocity() const {
	return particleInitialVelocity;
}

AnimatedProperty<float_t>& ParticleType::inheritedVelocity() {
	return particleInheritedVelocity;
}
const AnimatedProperty<float_t>& ParticleType::inheritedVelocity() const {
	return particleInheritedVelocity;
}

StaticProperty<float_t>& ParticleType::velocityVariance() {
	return particleVelocityVariance;
}
const StaticProperty<float_t>& ParticleType::velocityVariance() const {
	return particleVelocityVariance;
}

AnimatedProperty<float_t>& ParticleType::acceleration() {
	return particleAcceleration;
}
const AnimatedProperty<float_t>& ParticleType::acceleration() const {
	return particleAcceleration;
}

AnimatedProperty<float_t>& ParticleType::radialAcceleration() {
	return particleRadialAcceleration;
}
const AnimatedProperty<float_t>& ParticleType::radialAcceleration() const {
	return particleRadialAcceleration;
}

void ParticleType::rotationMode(RotationMode mode) {
	particleRotationMode = mode;
}
RotationMode ParticleType::rotationMode() const {
	return particleRotationMode;
}

void ParticleType::alignmentMode(AlignmentMode mode) {
	particleAlignmentMode = mode;
}
AlignmentMode ParticleType::alignmentMode() const {
	return particleAlignmentMode;
}

AnimatedProperty<float3_t>& ParticleType::initialRotation() {
	return particleInitialRotation;
}
const AnimatedProperty<float3_t>& ParticleType::initialRotation() const {
	return particleInitialRotation;
}

AnimatedProperty<float3_t>& ParticleType::rotation() {
	return particleRotation;
}
const AnimatedProperty<float3_t>& ParticleType::rotation() const {
	return particleRotation;
}

AnimatedProperty<float3_t>& ParticleType::rotationBySpeed() {
	return particleRotationBySpeed;
}
const AnimatedProperty<float3_t>& ParticleType::rotationBySpeed() const {
	return particleRotationBySpeed;
}

StaticProperty<float3_t>& ParticleType::rotationVariance() {
	return particleRotationVariance;
}
const StaticProperty<float3_t>& ParticleType::rotationVariance() const {
	return particleRotationVariance;
}

StaticProperty<float3_t>& ParticleType::angularVelocityVariance() {
	return particleAngularVelocityVariance;
}
const StaticProperty<float3_t>& ParticleType::angularVelocityVariance() const {
	return particleAngularVelocityVariance;
}

StaticProperty<float3_t>& ParticleType::pivot() {
	return particlePivot;
}
const StaticProperty<float3_t>& ParticleType::pivot() const {
	return particlePivot;
}

AnimatedProperty<float_t>& ParticleType::physicalSize() {
	return particlePhysicalSize;
}
const AnimatedProperty<float_t>& ParticleType::physicalSize() const {
	return particlePhysicalSize;
}

AnimatedProperty<float_t>& ParticleType::weight() {
	return particleWeight;
}
const AnimatedProperty<float_t>& ParticleType::weight() const {
	return particleWeight;
}

AnimatedProperty<float_t>& ParticleType::bounce() {
	return particleBounce;
}
const AnimatedProperty<float_t>& ParticleType::bounce() const {
	return particleBounce;
}

AnimatedProperty<float_t>& ParticleType::friction() {
	return particleFriction;
}
const AnimatedProperty<float_t>& ParticleType::friction() const {
	return particleFriction;
}

void ParticleType::visible(bool mode) {
	particleVisible = mode;
}
bool ParticleType::visible() const {
	return particleVisible;
}

void ParticleType::layer(uint32_t layer) {
	particleLayer = layer;
}
uint32_t ParticleType::layer() const {
	return particleLayer;
}

AnimatedProperty<float_t>& ParticleType::initialSize() {
	return particleInitialSize;
}
const AnimatedProperty<float_t>& ParticleType::initialSize() const {
	return particleInitialSize;
}

AnimatedProperty<float3_t>& ParticleType::size() {
	return particleSize;
}
const AnimatedProperty<float3_t>& ParticleType::size() const {
	return particleSize;
}

StaticProperty<float_t>& ParticleType::sizeVariance() {
	return particleSizeVariance;
}
const StaticProperty<float_t>& ParticleType::sizeVariance() const {
	return particleSizeVariance;
}

AnimatedProperty<float3_t>& ParticleType::stretch() {
	return particleStretch;
}
const AnimatedProperty<float3_t>& ParticleType::stretch() const {
	return particleStretch;
}

AnimatedProperty<float4_t>& ParticleType::color() {
	return particleColor;
}
const AnimatedProperty<float4_t>& ParticleType::color() const {
	return particleColor;
}

StaticProperty<float4_t>& ParticleType::colorVariance() {
	return particleColorVariance;
}
const StaticProperty<float4_t>& ParticleType::colorVariance() const {
	return particleColorVariance;
}

AnimatedProperty<float_t>& ParticleType::initialOpacity() {
	return particleInitialOpacity;
}
const AnimatedProperty<float_t>& ParticleType::initialOpacity() const {
	return particleInitialOpacity;
}

AnimatedProperty<float_t>& ParticleType::opacity() {
	return particleOpacity;
}
const AnimatedProperty<float_t>& ParticleType::opacity() const {
	return particleOpacity;
}

StaticProperty<float_t>& ParticleType::opacityVariance() {
	return particleOpacityVariance;
}
const StaticProperty<float_t>& ParticleType::opacityVariance() const {
	return particleOpacityVariance;
}

MaterialInstance& ParticleType::materialInstance() {
	return particleMaterialInstance;
}
const MaterialInstance& ParticleType::materialInstance() const {
	return particleMaterialInstance;
}

void ParticleType::renderer(ParticleRendererType renderer) {
	particleRenderer = renderer;
}
ParticleRendererType ParticleType::renderer() const {
	return particleRenderer;
}

ParticleSpriteRendererSettings& ParticleType::spriteRendererSettings() {
	return particleSpriteRendererSettings;
}
const ParticleSpriteRendererSettings& ParticleType::spriteRendererSettings() const {
	return particleSpriteRendererSettings;
}

ParticleTrailRendererSettings& ParticleType::trailRendererSettings() {
	return particleTrailRendererSettings;
}
const ParticleTrailRendererSettings& ParticleType::trailRendererSettings() const {
	return particleTrailRendererSettings;
}

ParticleMeshRendererSettings& ParticleType::meshRendererSettings() {
	return particleMeshRendererSettings;
}
const ParticleMeshRendererSettings& ParticleType::meshRendererSettings() const {
	return particleMeshRendererSettings;
}

void to_json(nlohmann::ordered_json& j, const ParticleType& particleType) {
	j = nlohmann::ordered_json{
		{ "id", particleType.id() },
		{ "parent_id", particleType.parentId() },
		{ "name", particleType.name() },
		{ "lifetime_start", particleType.start() },
		{ "lifetime_duration", particleType.duration() },
		{ "repeat", particleType.repeat() },
		{ "position", particleType.position() },

		{ "num_particles", particleType.count() },
		{ "lifespan", particleType.lifespan() },
		{ "lifespan_variance", particleType.lifespanVariance() },

		{ "position_relative", particleType.positionRelative() },
		{ "motion_path_force", particleType.motionPathForce() },
		{ "initial_velocity", particleType.initialVelocity() },
		{ "inherited_velocity", particleType.inheritedVelocity() },
		{ "velocity_variance", particleType.velocityVariance() },
		{ "acceleration", particleType.acceleration() },
		{ "radial_acceleration", particleType.radialAcceleration() },
		{ "rotation_mode", particleType.rotationMode() },
		{ "alignment_mode", particleType.alignmentMode() },
		{ "initial_rotation", particleType.initialRotation() },
		{ "rotation", particleType.rotation() },
		{ "rotation_by_speed", particleType.rotationBySpeed() },
		{ "rotation_variance", particleType.rotationVariance() },
		{ "angular_velocity_variance", particleType.angularVelocityVariance() },
		{ "pivot", particleType.pivot() },

		{ "physical_size", particleType.physicalSize() },
		{ "weight", particleType.weight() },
		{ "bounce", particleType.bounce() },
		{ "friction", particleType.friction() },

		{ "visible", particleType.visible() },
		{ "layer", particleType.layer() },
		{ "initial_size", particleType.initialSize() },
		{ "size", particleType.size() },
		{ "size_variance", particleType.sizeVariance() },
		{ "stretch", particleType.stretch() },
		{ "color", particleType.color() },
		{ "color_variance", particleType.colorVariance() },
		{ "initial_opacity", particleType.initialOpacity() },
		{ "opacity", particleType.opacity() },
		{ "opacity_variance", particleType.opacityVariance() },
		{ "material_instance", particleType.materialInstance() },
		{ "renderer", particleType.renderer() },
		{ "sprite_renderer_settings", particleType.spriteRendererSettings() },
		{ "trail_renderer_settings", particleType.trailRendererSettings() },
		{ "mesh_renderer_settings", particleType.meshRendererSettings() }
	};
}
void from_json(const nlohmann::ordered_json& j, ParticleType& particleType) {
	particleType = ParticleType(
		j.at("id"),
		j.value("parent_id", id_t()));

	particleType.name(j.value("name", ""));
	particleType.start(j.value("lifetime_start", 0.0));
	particleType.duration(j.value("lifetime_duration", 1.0));
	particleType.repeat(j.value("repeat", true));
	particleType.position() = j.value("position", AnimatedProperty<float3_t>(0.0, float3_t(0.0)));

	particleType.count() = j.value("num_particles", AnimatedProperty<float_t>(0.0));
	particleType.lifespan() = j.value("lifespan", AnimatedProperty<float_t>(1.0));
	particleType.lifespanVariance() = j.value("lifespan_variance", StaticProperty<float_t>(0.0));

	particleType.positionRelative(j.value("position_relative", false));
	particleType.motionPathForce() = j.value("motion_path_force", StaticProperty<float_t>(0.0));
	particleType.initialVelocity() = j.value("initial_velocity", AnimatedProperty<float_t>(1.0));
	particleType.inheritedVelocity() = j.value("inherited_velocity", AnimatedProperty<float_t>(0.0));
	particleType.velocityVariance() = j.value("velocity_variance",  StaticProperty<float_t>(0.0));
	particleType.acceleration() = j.value("acceleration", AnimatedProperty<float_t>(0.0));
	particleType.radialAcceleration() = j.value("radial_acceleration", AnimatedProperty<float_t>(0.0));
	particleType.rotationMode(j.value("rotation_mode", RotationMode::angle));
	particleType.alignmentMode(j.value("alignment_mode", AlignmentMode::camera));
	particleType.initialRotation() = j.value("initial_rotation", AnimatedProperty<float3_t>(float3_t(0.0)));
	particleType.rotation() = j.value("rotation", AnimatedProperty<float3_t>(float3_t(0.0)));
	particleType.rotationBySpeed() = j.value("rotation_by_speed", AnimatedProperty<float3_t>(float3_t(0.0)));
	particleType.rotationVariance() = j.value("rotation_variance", StaticProperty<float3_t>(float3_t(0.0)));
	particleType.angularVelocityVariance() = j.value("angular_velocity_variance", StaticProperty<float3_t>(float3_t(0.0)));
	particleType.pivot() = j.value("pivot", StaticProperty<float3_t>(float3_t(0.0)));

	particleType.physicalSize() = j.value("physical_size", AnimatedProperty<float_t>(1.0));
	particleType.weight() = j.value("weight", AnimatedProperty<float_t>(1.0));
	particleType.bounce() = j.value("bounce", AnimatedProperty<float_t>(1.0));
	particleType.friction() = j.value("friction", AnimatedProperty<float_t>(1.0));

	particleType.visible(j.value("visible", true));
	particleType.layer(j.value("layer", 0u));
	particleType.initialSize() = j.value("initial_size", AnimatedProperty<float_t>(0.1));
	particleType.size() = j.value("size", AnimatedProperty<float3_t>(float3_t(1.0)));
	particleType.sizeVariance() = j.value("size_variance", StaticProperty<float_t>(0.0));
	particleType.stretch() = j.value("stretch", AnimatedProperty<float3_t>(float3_t(0.0)));
	particleType.color() = j.value("color", AnimatedProperty<float4_t>(float4_t(1.0)));
	particleType.colorVariance() = j.value("color_variance", StaticProperty<float4_t>(float4_t(0.0)));
	particleType.initialOpacity() = j.value("initial_opacity", AnimatedProperty<float_t>(1.0));
	particleType.opacity() = j.value("opacity", AnimatedProperty<float_t>(1.0));
	particleType.opacityVariance() = j.value("opacity_variance", StaticProperty<float_t>(0.0));
	particleType.materialInstance() = j.value("material_instance", MaterialInstance());
	particleType.renderer(j.value("renderer", ParticleRendererType::sprite));
	particleType.spriteRendererSettings() = j.value("sprite_renderer_settings", ParticleSpriteRendererSettings());
	particleType.trailRendererSettings() = j.value("trail_renderer_settings", ParticleTrailRendererSettings());
	particleType.meshRendererSettings() = j.value("mesh_renderer_settings", ParticleMeshRendererSettings());
}
}