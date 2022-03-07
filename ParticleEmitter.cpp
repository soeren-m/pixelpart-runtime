#include "ParticleEmitter.h"
#include "JSONUtil.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const ParticleEmitter& emitter) {
	j = nlohmann::ordered_json{
		{ "name", emitter.name },
		{ "id", toJson(emitter.id) },
		{ "parent_id", toJson(emitter.parentId) },	
		{ "lifetime_start", emitter.lifetimeStart },
		{ "lifetime_duration", emitter.lifetimeDuration },
		{ "repeat", emitter.repeat },
		{ "shape", emitter.shape },
		{ "distribution", emitter.distribution },
		{ "spawn_mode", emitter.spawnMode },
		{ "width", emitter.width },
		{ "height", emitter.height },
		{ "orientation", emitter.orientation },
		{ "direction", emitter.direction },
		{ "spread", emitter.spread },
		{ "num_particles", emitter.numParticles },
		{ "burst", emitter.burst },
		{ "particle_lifespan", emitter.particleLifespan },
		{ "particle_lifespan_variance", emitter.particleLifespanVariance },	
		{ "particle_spawn_on_step_emitter", toJson(emitter.particleSpawnOnStepEmitterId) },
		{ "particle_spawn_on_step_prob", emitter.particleSpawnOnStepProb },
		{ "particle_spawn_on_death_emitter", toJson(emitter.particleSpawnOnDeathEmitterId) },
		{ "particle_spawn_on_death_number", emitter.particleSpawnOnDeathNumber },
		{ "particle_spawn_on_death_prob", emitter.particleSpawnOnDeathProb },
		{ "motion_path", emitter.motionPath },
		{ "particle_motion_path", emitter.particleMotionPath },
		{ "particle_motion_path_influence", emitter.particleMotionPathInfluence },
		{ "particle_position_relative", emitter.particlePositionRelative },
		{ "particle_initial_velocity", emitter.particleInitialVelocity },
		{ "particle_initial_velocity_variance", emitter.particleInitialVelocityVariance },
		{ "particle_acceleration", emitter.particleAcceleration },
		{ "particle_radial_acceleration", emitter.particleRadialAcceleration },
		{ "particle_damping", emitter.particleDamping },
		{ "particle_initial_rotation", emitter.particleInitialRotation },
		{ "particle_rotation", emitter.particleRotation },
		{ "particle_rotation_mode", emitter.particleRotationMode },
		{ "particle_rotation_variance", emitter.particleRotationVariance },
		{ "particle_weight", emitter.particleWeight },
		{ "particle_bounce", emitter.particleBounce },
		{ "particle_friction", emitter.particleFriction },
		{ "particle_sprite", emitter.particleSprite },
		{ "particle_sprite_animation", emitter.particleSpriteAnimation },
		{ "particle_initial_size", emitter.particleInitialSize },
		{ "particle_size", emitter.particleSize },
		{ "particle_width", emitter.particleWidth },
		{ "particle_height", emitter.particleHeight },
		{ "particle_size_variance", emitter.particleSizeVariance },
		{ "particle_color", emitter.particleColor },
		{ "particle_initial_opacity", emitter.particleInitialOpacity },
		{ "particle_opacity", emitter.particleOpacity },
		{ "particle_color_variance", emitter.particleColorVariance },
		{ "particle_opacity_variance", emitter.particleOpacityVariance },
		{ "alpha_threshold", emitter.alphaThreshold },
		{ "blend_mode", emitter.blendMode },
		{ "color_mode", emitter.colorMode },
		{ "renderer", emitter.renderer },
		{ "renderer_trail_uv_factor", emitter.rendererTrailUVFactor },
		{ "renderer_trail_uv_rotation", emitter.rendererTrailUVRotation },
		{ "renderer_num_trail_segments", emitter.rendererNumTrailSegments },
		{ "layer", emitter.layer },
		{ "visible", emitter.visible }
	};
}
void from_json(const nlohmann::ordered_json& j, ParticleEmitter& emitter) {
	const auto fromJsonMotionPath = [](Curve<vec2d>& path, const nlohmann::ordered_json& j, const std::string& key, floatd* duration = nullptr, floatd* influence = nullptr) {
		if(j.contains(key)) {
			const nlohmann::ordered_json& jValue = j.at(key);

			if(jValue.contains("waypoints")) {
				path = jValue.at("waypoints").get<Curve<vec2d>>();

				if(duration) {
					*duration = jValue.at("duration").get<floatd>();
				}
				if(influence) {
					*influence = jValue.at("influence").get<floatd>();
				}
			}
			else {
				fromJson(path, j, key, "");
			}
		}
	};

	emitter = ParticleEmitter();
	fromJson(emitter.name, j, "name", "");
	fromJson(emitter.id, j, "id", "");
	fromJson(emitter.parentId, j, "parent_id", "");	
	fromJson(emitter.lifetimeStart, j, "lifetime_start", "delay");
	fromJson(emitter.lifetimeDuration, j, "lifetime_duration", "duration");
	fromJson(emitter.repeat, j, "repeat", "");
	fromJson(emitter.shape, j, "shape", "");
	fromJson(emitter.distribution, j, "distribution", "");
	fromJson(emitter.spawnMode, j, "spawn_mode", "spawn_direction");
	fromJson(emitter.width, j, "width", "");
	fromJson(emitter.height, j, "height", "");
	fromJson(emitter.orientation, j, "orientation", "");
	fromJson(emitter.direction, j, "direction", "");
	fromJson(emitter.spread, j, "spread", "");
	fromJson(emitter.numParticles, j, "num_particles", "");
	fromJson(emitter.burst, j, "burst", "");	
	fromJson(emitter.particleLifespan, j, "particle_lifespan", "");
	fromJson(emitter.particleLifespanVariance, j, "particle_lifespan_variance", "");	
	fromJson(emitter.particleSpawnOnStepEmitterId, j, "particle_spawn_on_step_emitter", "");
	fromJson(emitter.particleSpawnOnStepProb, j, "particle_spawn_on_step_prob", "");
	fromJson(emitter.particleSpawnOnDeathEmitterId, j, "particle_spawn_on_death_emitter", "");
	fromJson(emitter.particleSpawnOnDeathNumber, j, "particle_spawn_on_death_number", "");
	fromJson(emitter.particleSpawnOnDeathProb, j, "particle_spawn_on_death_prob", "");

	fromJsonMotionPath(emitter.motionPath, j, "motion_path", nullptr);
	fromJsonMotionPath(emitter.particleMotionPath, j, "particle_motion_path", nullptr, &emitter.particleMotionPathInfluence);
	fromJson(emitter.particleMotionPathInfluence, j, "particle_motion_path_influence", "");
	fromJson(emitter.particlePositionRelative, j, "particle_position_relative", "");
	fromJson(emitter.particleInitialVelocity, j, "particle_initial_velocity", "");
	fromJson(emitter.particleInitialVelocityVariance, j, "particle_initial_velocity_variance", "");
	fromJson(emitter.particleAcceleration, j, "particle_acceleration", "");
	fromJson(emitter.particleRadialAcceleration, j, "particle_radial_acceleration", "");
	fromJson(emitter.particleDamping, j, "particle_damping", "");
	fromJson(emitter.particleInitialRotation, j, "particle_initial_rotation", "");
	fromJson(emitter.particleRotation, j, "particle_rotation", "");
	fromJson(emitter.particleRotationMode, j, "particle_rotation_mode", "");
	fromJson(emitter.particleRotationVariance, j, "particle_rotation_variance", "");

	if(j.contains("particle_rotation_offset")) {
		emitter.particleInitialRotation = Curve<floatd>(j.at("particle_rotation_offset").get<floatd>());
	}

	fromJson(emitter.particleWeight, j, "particle_weight", "");
	fromJson(emitter.particleBounce, j, "particle_bounce", "");
	fromJson(emitter.particleFriction, j, "particle_friction", "");
	
	if(j.contains("particle_sprite") && j.contains("particle_sprite_animation")) {
		fromJson(emitter.particleSprite, j, "particle_sprite", "");
		fromJson(emitter.particleSpriteAnimation, j, "particle_sprite_animation", "");
	}
	else {
		fromJson(emitter.particleSprite.id, j, "sprite_texture_id", "");
		fromJson(emitter.particleSprite.texturePivot, j, "sprite_texture_pivot", "");
		fromJson(emitter.particleSprite.textureOrigin, j, "sprite_texture_origin", "");
		fromJson(emitter.particleSprite.framesRow, j, "sprite_rows", "");
		fromJson(emitter.particleSprite.framesColumn, j, "sprite_columns", "");
		fromJson(emitter.particleSprite.frame, j, "sprite_frame", "");
		fromJson(emitter.particleSprite.randomFrame, j, "sprite_random_frame", "");
		fromJson(emitter.particleSpriteAnimation.frames, j, "sprite_animation_frames", "");				
		fromJson(emitter.particleSpriteAnimation.duration, j, "sprite_animation_duration", "");				
		fromJson(emitter.particleSpriteAnimation.loop, j, "sprite_animation_loop", "");
		if(j.contains("sprite_animated")) {
			emitter.particleSpriteAnimation.frames = j.at("sprite_animated").get<bool>() ? emitter.particleSpriteAnimation.frames : 1;
		}
	}

	fromJson(emitter.particleInitialSize, j, "particle_initial_size", "");
	fromJson(emitter.particleSize, j, "particle_size", "");
	fromJson(emitter.particleWidth, j, "particle_width", "");
	fromJson(emitter.particleHeight, j, "particle_height", "");
	fromJson(emitter.particleSizeVariance, j, "particle_size_variance", "");
	fromJson(emitter.particleColor, j, "particle_color", "");
	fromJson(emitter.particleInitialOpacity, j, "particle_initial_opacity", "");
	fromJson(emitter.particleOpacity, j, "particle_opacity", "");
	fromJson(emitter.particleColorVariance, j, "particle_color_variance", "");
	fromJson(emitter.particleOpacityVariance, j, "particle_opacity_variance", "");
	fromJson(emitter.alphaThreshold, j, "alpha_threshold", "");
	fromJson(emitter.blendMode, j, "blend_mode", "");
	fromJson(emitter.colorMode, j, "color_mode", "");
	fromJson(emitter.renderer, j, "renderer", "");
	fromJson(emitter.rendererTrailUVFactor, j, "renderer_trail_uv_factor", "");
	fromJson(emitter.rendererTrailUVRotation, j, "renderer_trail_uv_rotation", "");
	fromJson(emitter.rendererNumTrailSegments, j, "renderer_num_trail_segments", "");
	fromJson(emitter.layer, j, "layer", "");
	fromJson(emitter.visible, j, "visible", "");

	if(!j.contains("particle_opacity")) {
		emitter.particleOpacity.clear();
		for(Curve<vec4d>::Point& point : emitter.particleColor.getPoints()) {
			emitter.particleOpacity.addPoint(point.position, point.value.a);
			point.value.a = 1.0;
		}

		emitter.particleColor.refreshCache();
	}
}
}