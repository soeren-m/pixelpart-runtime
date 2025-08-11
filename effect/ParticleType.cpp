#include "ParticleType.h"

namespace pixelpart {
ParticleType::ParticleType(id_t ownId) : particleTypeId(ownId) {

}

void ParticleType::applyInputs(const ComputeGraph::InputSet& inputs) {
	particlePivot.input(inputs);
	particleMotionPathForce.input(inputs);

	particleCount.input(inputs);
	particleLifespan.input(inputs);
	particleInitialSize.input(inputs);
	particleInitialRotation.input(inputs);
	particleInitialVelocity.input(inputs);
	particleInheritedVelocity.input(inputs);
	particleInitialOpacity.input(inputs);

	particleLifetimeMotionPath.input(inputs);
	particleLifetimeSize.input(inputs);
	particleLifetimeStretch.input(inputs);
	particleLifetimePhysicalSize.input(inputs);
	particleLifetimeRotation.input(inputs);
	particleLifetimeRotationBySpeed.input(inputs);
	particleLifetimeAcceleration.input(inputs);
	particleLifetimeRadialAcceleration.input(inputs);
	particleLifetimeWeight.input(inputs);
	particleLifetimeBounce.input(inputs);
	particleLifetimeFriction.input(inputs);
	particleLifetimeColor.input(inputs);
	particleLifetimeOpacity.input(inputs);

	particleLifespanVariance.input(inputs);
	particleSizeVariance.input(inputs);
	particleRotationVariance.input(inputs);
	particleAngularVelocityVariance.input(inputs);
	particleVelocityVariance.input(inputs);
	particleColorVariance.input(inputs);
	particleOpacityVariance.input(inputs);
}
bool ParticleType::usesResource(const std::string& resourceId) const {
	if(resourceId == particleMaterialInstance.materialId()) {
		return true;
	}

	if(resourceId == particleMeshRendererSettings.meshResourceId) {
		return true;
	}

	for(const auto& materialParameterEntry : particleMaterialInstance.materialParameters()) {
		if(materialParameterEntry.second.type() == pixelpart::VariantParameter::Value::type_resource_image &&
			resourceId == materialParameterEntry.second.valueResourceId()) {
			return true;
		}
	}

	return false;
}

id_t ParticleType::id() const {
	return particleTypeId;
}

void ParticleType::parent(id_t parentId) {
	parentParticleTypeId = parentId;
}
id_t ParticleType::parentId() const {
	return parentParticleTypeId;
}

void ParticleType::name(const std::string& name) {
	particleTypeName = name;
}
const std::string& ParticleType::name() const {
	return particleTypeName;
}

void ParticleType::positionRelative(bool relative) {
	particlePositionRelative = relative;
}
bool ParticleType::positionRelative() const {
	return particlePositionRelative;
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

StaticProperty<float3_t>& ParticleType::pivot() {
	return particlePivot;
}
const StaticProperty<float3_t>& ParticleType::pivot() const {
	return particlePivot;
}

StaticProperty<float_t>& ParticleType::motionPathForce() {
	return particleMotionPathForce;
}
const StaticProperty<float_t>& ParticleType::motionPathForce() const {
	return particleMotionPathForce;
}

void ParticleType::visible(bool mode) {
	particleVisible = mode;
}
bool ParticleType::visible() const {
	return particleVisible;
}

void ParticleType::layer(std::uint32_t layer) {
	particleLayer = layer;
}
std::uint32_t ParticleType::layer() const {
	return particleLayer;
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

AnimatedProperty<float_t>& ParticleType::initialSize() {
	return particleInitialSize;
}
const AnimatedProperty<float_t>& ParticleType::initialSize() const {
	return particleInitialSize;
}

AnimatedProperty<float3_t>& ParticleType::initialRotation() {
	return particleInitialRotation;
}
const AnimatedProperty<float3_t>& ParticleType::initialRotation() const {
	return particleInitialRotation;
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

AnimatedProperty<float_t>& ParticleType::initialOpacity() {
	return particleInitialOpacity;
}
const AnimatedProperty<float_t>& ParticleType::initialOpacity() const {
	return particleInitialOpacity;
}

AnimatedProperty<float3_t>& ParticleType::motionPath() {
	return particleLifetimeMotionPath;
}
const AnimatedProperty<float3_t>& ParticleType::motionPath() const {
	return particleLifetimeMotionPath;
}

AnimatedProperty<float3_t>& ParticleType::size() {
	return particleLifetimeSize;
}
const AnimatedProperty<float3_t>& ParticleType::size() const {
	return particleLifetimeSize;
}

AnimatedProperty<float3_t>& ParticleType::stretch() {
	return particleLifetimeStretch;
}
const AnimatedProperty<float3_t>& ParticleType::stretch() const {
	return particleLifetimeStretch;
}

AnimatedProperty<float_t>& ParticleType::physicalSize() {
	return particleLifetimePhysicalSize;
}
const AnimatedProperty<float_t>& ParticleType::physicalSize() const {
	return particleLifetimePhysicalSize;
}

AnimatedProperty<float3_t>& ParticleType::rotation() {
	return particleLifetimeRotation;
}
const AnimatedProperty<float3_t>& ParticleType::rotation() const {
	return particleLifetimeRotation;
}

AnimatedProperty<float3_t>& ParticleType::rotationBySpeed() {
	return particleLifetimeRotationBySpeed;
}
const AnimatedProperty<float3_t>& ParticleType::rotationBySpeed() const {
	return particleLifetimeRotationBySpeed;
}

AnimatedProperty<float_t>& ParticleType::acceleration() {
	return particleLifetimeAcceleration;
}
const AnimatedProperty<float_t>& ParticleType::acceleration() const {
	return particleLifetimeAcceleration;
}

AnimatedProperty<float_t>& ParticleType::radialAcceleration() {
	return particleLifetimeRadialAcceleration;
}
const AnimatedProperty<float_t>& ParticleType::radialAcceleration() const {
	return particleLifetimeRadialAcceleration;
}

AnimatedProperty<float_t>& ParticleType::weight() {
	return particleLifetimeWeight;
}
const AnimatedProperty<float_t>& ParticleType::weight() const {
	return particleLifetimeWeight;
}

AnimatedProperty<float_t>& ParticleType::bounce() {
	return particleLifetimeBounce;
}
const AnimatedProperty<float_t>& ParticleType::bounce() const {
	return particleLifetimeBounce;
}

AnimatedProperty<float_t>& ParticleType::friction() {
	return particleLifetimeFriction;
}
const AnimatedProperty<float_t>& ParticleType::friction() const {
	return particleLifetimeFriction;
}

AnimatedProperty<float4_t>& ParticleType::color() {
	return particleLifetimeColor;
}
const AnimatedProperty<float4_t>& ParticleType::color() const {
	return particleLifetimeColor;
}

AnimatedProperty<float_t>& ParticleType::opacity() {
	return particleLifetimeOpacity;
}
const AnimatedProperty<float_t>& ParticleType::opacity() const {
	return particleLifetimeOpacity;
}

StaticProperty<float_t>& ParticleType::lifespanVariance() {
	return particleLifespanVariance;
}
const StaticProperty<float_t>& ParticleType::lifespanVariance() const {
	return particleLifespanVariance;
}

StaticProperty<float_t>& ParticleType::sizeVariance() {
	return particleSizeVariance;
}
const StaticProperty<float_t>& ParticleType::sizeVariance() const {
	return particleSizeVariance;
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

StaticProperty<float_t>& ParticleType::velocityVariance() {
	return particleVelocityVariance;
}
const StaticProperty<float_t>& ParticleType::velocityVariance() const {
	return particleVelocityVariance;
}

StaticProperty<float4_t>& ParticleType::colorVariance() {
	return particleColorVariance;
}
const StaticProperty<float4_t>& ParticleType::colorVariance() const {
	return particleColorVariance;
}

StaticProperty<float_t>& ParticleType::opacityVariance() {
	return particleOpacityVariance;
}
const StaticProperty<float_t>& ParticleType::opacityVariance() const {
	return particleOpacityVariance;
}

void to_json(nlohmann::ordered_json& j, const ParticleType& particleType) {
	j = nlohmann::ordered_json{
		{ "id", particleType.id() },
		{ "parent_id", particleType.parentId() },
		{ "name", particleType.name() },

		{ "position_relative", particleType.positionRelative() },
		{ "rotation_mode", particleType.rotationMode() },
		{ "alignment_mode", particleType.alignmentMode() },

		{ "pivot", particleType.pivot() },
		{ "motion_path_force", particleType.motionPathForce() },

		{ "visible", particleType.visible() },
		{ "layer", particleType.layer() },
		{ "material_instance", particleType.materialInstance() },
		{ "renderer", particleType.renderer() },
		{ "sprite_renderer_settings", particleType.spriteRendererSettings() },
		{ "trail_renderer_settings", particleType.trailRendererSettings() },
		{ "mesh_renderer_settings", particleType.meshRendererSettings() },

		{ "count", particleType.count() },
		{ "lifespan", particleType.lifespan() },
		{ "initial_size", particleType.initialSize() },
		{ "initial_rotation", particleType.initialRotation() },
		{ "initial_velocity", particleType.initialVelocity() },
		{ "inherited_velocity", particleType.inheritedVelocity() },
		{ "initial_opacity", particleType.initialOpacity() },

		{ "position", particleType.motionPath() },
		{ "size", particleType.size() },
		{ "stretch", particleType.stretch() },
		{ "physical_size", particleType.physicalSize() },
		{ "rotation", particleType.rotation() },
		{ "rotation_by_speed", particleType.rotationBySpeed() },
		{ "acceleration", particleType.acceleration() },
		{ "radial_acceleration", particleType.radialAcceleration() },
		{ "weight", particleType.weight() },
		{ "bounce", particleType.bounce() },
		{ "friction", particleType.friction() },
		{ "color", particleType.color() },
		{ "opacity", particleType.opacity() },

		{ "lifespan_variance", particleType.lifespanVariance() },
		{ "size_variance", particleType.sizeVariance() },
		{ "rotation_variance", particleType.rotationVariance() },
		{ "angular_velocity_variance", particleType.angularVelocityVariance() },
		{ "velocity_variance", particleType.velocityVariance() },
		{ "color_variance", particleType.colorVariance() },
		{ "opacity_variance", particleType.opacityVariance() }
	};
}
void from_json(const nlohmann::ordered_json& j, ParticleType& particleType) {
	particleType = ParticleType(j.at("id").get<id_t>());
	particleType.parent(j.value("parent_id", id_t()));
	particleType.name(j.value("name", ""));

	particleType.positionRelative(j.value("position_relative", false));
	particleType.rotationMode(j.value("rotation_mode", RotationMode::angle));
	particleType.alignmentMode(j.value("alignment_mode", AlignmentMode::camera));

	particleType.pivot() = j.value("pivot", StaticProperty<float3_t>(float3_t(0.0)));
	particleType.motionPathForce() = j.value("motion_path_force", StaticProperty<float_t>(0.0));

	particleType.visible(j.value("visible", true));
	particleType.layer(j.value("layer", 0u));
	particleType.materialInstance() = j.value("material_instance", MaterialInstance());
	particleType.renderer(j.value("renderer", ParticleRendererType::sprite));
	particleType.spriteRendererSettings() = j.value("sprite_renderer_settings", ParticleSpriteRendererSettings());
	particleType.trailRendererSettings() = j.value("trail_renderer_settings", ParticleTrailRendererSettings());
	particleType.meshRendererSettings() = j.value("mesh_renderer_settings", ParticleMeshRendererSettings());

	particleType.count() = j.value("count", AnimatedProperty<float_t>(0.0));
	particleType.lifespan() = j.value("lifespan", AnimatedProperty<float_t>(1.0));
	particleType.initialSize() = j.value("initial_size", AnimatedProperty<float_t>(0.1));
	particleType.initialRotation() = j.value("initial_rotation", AnimatedProperty<float3_t>(float3_t(0.0)));
	particleType.initialVelocity() = j.value("initial_velocity", AnimatedProperty<float_t>(1.0));
	particleType.inheritedVelocity() = j.value("inherited_velocity", AnimatedProperty<float_t>(0.0));
	particleType.initialOpacity() = j.value("initial_opacity", AnimatedProperty<float_t>(1.0));

	particleType.motionPath() = j.value("position", AnimatedProperty<float3_t>(0.0, float3_t(0.0)));
	particleType.size() = j.value("size", AnimatedProperty<float3_t>(float3_t(1.0)));
	particleType.stretch() = j.value("stretch", AnimatedProperty<float3_t>(float3_t(0.0)));
	particleType.physicalSize() = j.value("physical_size", AnimatedProperty<float_t>(1.0));
	particleType.rotation() = j.value("rotation", AnimatedProperty<float3_t>(float3_t(0.0)));
	particleType.rotationBySpeed() = j.value("rotation_by_speed", AnimatedProperty<float3_t>(float3_t(0.0)));
	particleType.acceleration() = j.value("acceleration", AnimatedProperty<float_t>(0.0));
	particleType.radialAcceleration() = j.value("radial_acceleration", AnimatedProperty<float_t>(0.0));
	particleType.weight() = j.value("weight", AnimatedProperty<float_t>(1.0));
	particleType.bounce() = j.value("bounce", AnimatedProperty<float_t>(1.0));
	particleType.friction() = j.value("friction", AnimatedProperty<float_t>(1.0));
	particleType.color() = j.value("color", AnimatedProperty<float4_t>(float4_t(1.0)));
	particleType.opacity() = j.value("opacity", AnimatedProperty<float_t>(1.0));

	particleType.lifespanVariance() = j.value("lifespan_variance", StaticProperty<float_t>(0.0));
	particleType.sizeVariance() = j.value("size_variance", StaticProperty<float_t>(0.0));
	particleType.rotationVariance() = j.value("rotation_variance", StaticProperty<float3_t>(float3_t(0.0)));
	particleType.angularVelocityVariance() = j.value("angular_velocity_variance", StaticProperty<float3_t>(float3_t(0.0)));
	particleType.velocityVariance() = j.value("velocity_variance", StaticProperty<float_t>(0.0));
	particleType.colorVariance() = j.value("color_variance", StaticProperty<float4_t>(float4_t(0.0)));
	particleType.opacityVariance() = j.value("opacity_variance", StaticProperty<float_t>(0.0));
}
}
