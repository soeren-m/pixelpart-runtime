#include "ParticleEmitter.h"
#include "ShaderGraphBuilder.h"
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
		{ "instantiation_mode", emitter.instantiationMode },
		{ "shape_path", emitter.shapePath },
		{ "width", emitter.width },
		{ "height", emitter.height },
		{ "orientation", emitter.orientation },
		{ "direction", emitter.direction },
		{ "spread", emitter.spread },
		{ "num_particles", emitter.numParticles },
		{ "particle_lifespan", emitter.particleLifespan },
		{ "particle_lifespan_variance", emitter.particleLifespanVariance },
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
		{ "particle_pivot", emitter.particlePivot },
		{ "particle_weight", emitter.particleWeight },
		{ "particle_bounce", emitter.particleBounce },
		{ "particle_friction", emitter.particleFriction },
		{ "particle_shader", emitter.particleShader },
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
		{ "blend_mode", emitter.blendMode },
		{ "renderer", emitter.renderer },
		{ "renderer_trail_uv_factor", emitter.rendererTrailUVFactor },
		{ "renderer_trail_uv_rotation", emitter.rendererTrailUVRotation },
		{ "renderer_num_trail_segments", emitter.rendererNumTrailSegments },
		{ "layer", emitter.layer },
		{ "visible", emitter.visible }
	};
}
void from_json(const nlohmann::ordered_json& j, ParticleEmitter& emitter) {
	const auto fromJsonMotionPath = [](Curve<vec2d>& path, const nlohmann::ordered_json& j, const std::string& key) {
		if(j.contains(key)) {
			const nlohmann::ordered_json& jValue = j.at(key);

			if(jValue.contains("waypoints")) {
				path = jValue.at("waypoints").get<Curve<vec2d>>();
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
	fromJson(emitter.instantiationMode, j, "instantiation_mode", "");
	fromJson(emitter.shapePath, j, "shape_path", "");
	fromJson(emitter.width, j, "width", "");
	fromJson(emitter.height, j, "height", "");
	fromJson(emitter.orientation, j, "orientation", "");
	fromJson(emitter.direction, j, "direction", "");
	fromJson(emitter.spread, j, "spread", "");
	fromJson(emitter.numParticles, j, "num_particles", "");
	fromJson(emitter.particleLifespan, j, "particle_lifespan", "");
	fromJson(emitter.particleLifespanVariance, j, "particle_lifespan_variance", "");

	fromJsonMotionPath(emitter.motionPath, j, "motion_path");
	fromJsonMotionPath(emitter.particleMotionPath, j, "particle_motion_path");
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
	fromJson(emitter.particlePivot, j, "particle_pivot", "");

	fromJson(emitter.particleWeight, j, "particle_weight", "");
	fromJson(emitter.particleBounce, j, "particle_bounce", "");
	fromJson(emitter.particleFriction, j, "particle_friction", "");

	fromJson(emitter.particleShader, j, "particle_shader", "");
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
	fromJson(emitter.blendMode, j, "blend_mode", "");
	fromJson(emitter.renderer, j, "renderer", "");
	fromJson(emitter.rendererTrailUVFactor, j, "renderer_trail_uv_factor", "");
	fromJson(emitter.rendererTrailUVRotation, j, "renderer_trail_uv_rotation", "");
	fromJson(emitter.rendererNumTrailSegments, j, "renderer_num_trail_segments", "");
	fromJson(emitter.layer, j, "layer", "");
	fromJson(emitter.visible, j, "visible", "");

	if(j.contains("burst")) {
		emitter.instantiationMode = j.at("burst").get<bool>()
			? ParticleEmitter::InstantiationMode::burst
			: ParticleEmitter::InstantiationMode::continuous;
	}

	if(j.contains("particle_rotation_offset")) {
		emitter.particleInitialRotation = Curve<floatd>(j.at("particle_rotation_offset").get<floatd>());
	}

	if(j.contains("particle_sprite")) {
		const nlohmann::ordered_json& jSprite = j.at("particle_sprite");
		const nlohmann::ordered_json& jAnimation = j.at("particle_sprite_animation");

		std::string textureId;
		uint32_t numFramesRow = 1;
		uint32_t numFramesColumn = 1;
		uint32_t numAnimationFrames = 1;
		fromJson(textureId, jSprite, "id", "");
		fromJson(numFramesRow, jSprite, "frames_row", "");
		fromJson(numFramesColumn, jSprite, "frames_column", "");
		fromJson(numAnimationFrames, jAnimation, "frames", "");

		if(jSprite.contains("texture_pivot")) {
			emitter.particlePivot = jSprite.at("texture_pivot");
		}

		if(numFramesRow > 1 || numFramesColumn > 1) {
			if(numAnimationFrames > 1) {
				emitter.particleShader = ShaderGraphBuilder::buildSpriteSheetAnimationShaderGraph(textureId, "",
					numFramesRow, numFramesColumn,
					jSprite.contains("texture_origin") ? jSprite.at("texture_origin").get<uint32_t>() : 0u,
					jSprite.contains("frame") ? jSprite.at("frame").get<uint32_t>() : 0u,
					numAnimationFrames,
					jAnimation.contains("duration") ? jAnimation.at("duration").get<floatd>() : 1.0,
					jAnimation.contains("loop") ? jAnimation.at("loop").get<bool>() : false,
					j.contains("color_mode") ? j.at("color_mode").get<uint32_t>() : 0u,
					j.contains("alpha_threshold") ? j.at("alpha_threshold").get<floatd>() : 0.0);
			}
			else {
				emitter.particleShader = ShaderGraphBuilder::buildSpriteSheetStaticShaderGraph(textureId, "",
					numFramesRow, numFramesColumn,
					jSprite.contains("texture_origin") ? jSprite.at("texture_origin").get<uint32_t>() : 0u,
					jSprite.contains("frame") ? jSprite.at("frame").get<uint32_t>() : 0u,
					jSprite.contains("random_frame") ? jSprite.at("random_frame").get<bool>() : false,
					j.contains("color_mode") ? j.at("color_mode").get<uint32_t>() : 0u,
					j.contains("alpha_threshold") ? j.at("alpha_threshold").get<floatd>() : 0.0);
			}
		}
		else {
			emitter.particleShader = ShaderGraphBuilder::buildTextureShaderGraph(textureId, "",
				j.contains("color_mode") ? j.at("color_mode").get<uint32_t>() : 0u,
				j.contains("alpha_threshold") ? j.at("alpha_threshold").get<floatd>() : 0.0);
		}
	}
	else if(j.contains("sprite_texture_id")) {
		std::string textureId;
		uint32_t numFramesRow = 1;
		uint32_t numFramesColumn = 1;
		bool animated = true;
		fromJson(textureId, j, "sprite_texture_id", "");
		fromJson(numFramesRow, j, "sprite_rows", "");
		fromJson(numFramesColumn, j, "sprite_columns", "");
		fromJson(animated, j, "sprite_animated", "");

		if(j.contains("sprite_texture_pivot")) {
			emitter.particlePivot = j.at("sprite_texture_pivot");
		}

		if(numFramesRow > 1 || numFramesColumn > 1) {
			if(animated) {
				emitter.particleShader = ShaderGraphBuilder::buildSpriteSheetAnimationShaderGraph(textureId, "",
					numFramesRow,
					numFramesColumn,
					j.contains("sprite_texture_origin") ? j.at("sprite_texture_origin").get<uint32_t>() : 0u,
					j.contains("sprite_frame") ? j.at("sprite_frame").get<uint32_t>() : 0u,
					j.contains("sprite_animation_frames") ? j.at("sprite_animation_frames").get<uint32_t>() : 1u,
					j.contains("sprite_animation_duration") ? j.at("sprite_animation_duration").get<floatd>() : 1.0,
					j.contains("sprite_animation_loop") ? j.at("sprite_animation_loop").get<bool>() : false,
					j.contains("color_mode") ? j.at("color_mode").get<uint32_t>() : 0u,
					j.contains("alpha_threshold") ? j.at("alpha_threshold").get<floatd>() : 0.0);
			}
			else {
				emitter.particleShader = ShaderGraphBuilder::buildSpriteSheetStaticShaderGraph(textureId, "",
					numFramesRow,
					numFramesColumn,
					j.contains("sprite_texture_origin") ? j.at("sprite_texture_origin").get<uint32_t>() : 0u,
					j.contains("sprite_frame") ? j.at("sprite_frame").get<uint32_t>() : 0u,
					j.contains("sprite_random_frame") ? j.at("sprite_random_frame").get<bool>() : false,
					j.contains("color_mode") ? j.at("color_mode").get<uint32_t>() : 0u,
					j.contains("alpha_threshold") ? j.at("alpha_threshold").get<floatd>() : 0.0);
			}
		}
		else {
			emitter.particleShader = ShaderGraphBuilder::buildTextureShaderGraph(textureId, "",
				j.contains("color_mode") ? j.at("color_mode").get<uint32_t>() : 0u,
				j.contains("alpha_threshold") ? j.at("alpha_threshold").get<floatd>() : 0.0);
		}
	}

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