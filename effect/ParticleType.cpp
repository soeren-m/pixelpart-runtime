#include "ParticleType.h"
#include "../common/JsonUtil.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const ParticleType& particleType) {
	j = nlohmann::ordered_json{
		{ "id", toJson(particleType.id) },
		{ "parent_id", toJson(particleType.parentId) },
		{ "name", particleType.name },
		{ "lifetime_start", particleType.lifetimeStart },
		{ "lifetime_duration", particleType.lifetimeDuration },
		{ "repeat", particleType.repeat },
		{ "position", particleType.position },

		{ "num_particles", particleType.numParticles },
		{ "lifespan", particleType.lifespan },
		{ "lifespan_variance", particleType.lifespanVariance },

		{ "position_relative", particleType.positionRelative },
		{ "motion_path_force", particleType.motionPathForce },
		{ "initial_velocity", particleType.initialVelocity },
		{ "inherited_velocity", particleType.inheritedVelocity },
		{ "velocity_variance", particleType.velocityVariance },
		{ "acceleration", particleType.acceleration },
		{ "radial_acceleration", particleType.radialAcceleration },
		{ "damping", particleType.damping },
		{ "rotation_mode", particleType.rotationMode },
		{ "alignment_mode", particleType.alignmentMode },
		{ "initial_rotation", particleType.initialRotation },
		{ "rotation", particleType.rotation },
		{ "rotation_by_speed", particleType.rotationBySpeed },
		{ "rotation_variance", particleType.rotationVariance },
		{ "angular_velocity_variance", particleType.angularVelocityVariance },
		{ "pivot", particleType.pivot },

		{ "weight", particleType.weight },
		{ "bounce", particleType.bounce },
		{ "friction", particleType.friction },

		{ "visible", particleType.visible },
		{ "layer", particleType.layer },
		{ "blend_mode", particleType.blendMode },
		{ "shader", particleType.shader },
		{ "initial_size", particleType.initialSize },
		{ "size", particleType.size },
		{ "size_variance", particleType.sizeVariance },
		{ "stretch", particleType.stretch },
		{ "color", particleType.color },
		{ "color_variance", particleType.colorVariance },
		{ "initial_opacity", particleType.initialOpacity },
		{ "opacity", particleType.opacity },
		{ "opacity_variance", particleType.opacityVariance },

		{ "renderer", particleType.renderer },
		{ "sprite_renderer_settings", particleType.spriteRendererSettings },
		{ "trail_renderer_settings", particleType.trailRendererSettings }
	};
}
void from_json(const nlohmann::ordered_json& j, ParticleType& particleType) {
	particleType = ParticleType();

	fromJson(particleType.id, j, "id");
	fromJson(particleType.parentId, j, "parent_id");
	fromJson(particleType.name, j, "name");
	fromJson(particleType.lifetimeStart, j, "lifetime_start");
	fromJson(particleType.lifetimeDuration, j, "lifetime_duration");
	fromJson(particleType.repeat, j, "repeat");
	fromJson(particleType.position, j, "position");

	fromJson(particleType.numParticles, j, "num_particles");
	fromJson(particleType.lifespan, j, "lifespan");
	fromJson(particleType.lifespanVariance, j, "lifespan_variance");

	fromJson(particleType.positionRelative, j, "position_relative");
	fromJson(particleType.motionPathForce, j, "motion_path_force");
	fromJson(particleType.initialVelocity, j, "initial_velocity");
	fromJson(particleType.inheritedVelocity, j, "inherited_velocity");
	fromJson(particleType.velocityVariance, j, "velocity_variance");
	fromJson(particleType.acceleration, j, "acceleration");
	fromJson(particleType.radialAcceleration, j, "radial_acceleration");
	fromJson(particleType.damping, j, "damping");
	fromJson(particleType.rotationMode, j, "rotation_mode");
	fromJson(particleType.alignmentMode, j, "alignment_mode");
	fromJson(particleType.initialRotation, j, "initial_rotation");
	fromJson(particleType.rotation, j, "rotation");
	fromJson(particleType.rotationBySpeed, j, "rotation_by_speed");
	fromJson(particleType.rotationVariance, j, "rotation_variance");
	fromJson(particleType.angularVelocityVariance, j, "angular_velocity_variance");
	fromJson(particleType.pivot, j, "pivot");

	fromJson(particleType.weight, j, "weight");
	fromJson(particleType.bounce, j, "bounce");
	fromJson(particleType.friction, j, "friction");

	fromJson(particleType.visible, j, "visible");
	fromJson(particleType.layer, j, "layer");
	fromJson(particleType.blendMode, j, "blend_mode");
	fromJson(particleType.shader, j, "shader");
	fromJson(particleType.initialSize, j, "initial_size");
	fromJson(particleType.size, j, "size");
	fromJson(particleType.sizeVariance, j, "size_variance");
	fromJson(particleType.stretch, j, "stretch");
	fromJson(particleType.color, j, "color");
	fromJson(particleType.colorVariance, j, "color_variance");
	fromJson(particleType.initialOpacity, j, "initial_opacity");
	fromJson(particleType.opacity, j, "opacity");
	fromJson(particleType.opacityVariance, j, "opacity_variance");

	fromJson(particleType.renderer, j, "renderer");
	fromJson(particleType.spriteRendererSettings, j, "sprite_renderer_settings");
	fromJson(particleType.trailRendererSettings, j, "trail_renderer_settings");
}
}