#include "Sprite.h"
#include "ShaderGraphBuilder.h"
#include "JSONUtil.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const Sprite& sprite) {
	j = nlohmann::ordered_json{
		{ "name", sprite.name },
		{ "lifetime_start", sprite.lifetimeStart },
		{ "lifetime_duration", sprite.lifetimeDuration },
		{ "repeat", sprite.repeat },
		{ "motion_path", sprite.motionPath },
		{ "width", sprite.width },
		{ "height", sprite.height },
		{ "orientation", sprite.orientation },
		{ "align_with_path", sprite.alignWithPath },
		{ "pivot", sprite.pivot },
		{ "shader", sprite.shader },
		{ "color", sprite.color },
		{ "opacity", sprite.opacity },
		{ "blend_mode", sprite.blendMode },
		{ "layer", sprite.layer },
		{ "visible", sprite.visible }
	};
}
void from_json(const nlohmann::ordered_json& j, Sprite& sprite) {
	sprite = Sprite();
	fromJson(sprite.name, j, "name", "");
	fromJson(sprite.lifetimeStart, j, "lifetime_start", "delay");
	fromJson(sprite.lifetimeDuration, j, "lifetime_duration", "duration");
	fromJson(sprite.repeat, j, "repeat", "");
	fromJson(sprite.motionPath, j, "motion_path", "");
	fromJson(sprite.width, j, "width", "");
	fromJson(sprite.height, j, "height", "");
	fromJson(sprite.orientation, j, "orientation", "");
	fromJson(sprite.alignWithPath, j, "align_with_path", "");
	fromJson(sprite.pivot, j, "pivot", "");
	fromJson(sprite.shader, j, "shader", "");
	fromJson(sprite.color, j, "color", "");
	fromJson(sprite.opacity, j, "opacity", "");
	fromJson(sprite.blendMode, j, "blend_mode", "");
	fromJson(sprite.layer, j, "layer", "");
	fromJson(sprite.visible, j, "visible", "");

	if(j.contains("image")) {
		const nlohmann::ordered_json& jImage = j.at("image");
		const nlohmann::ordered_json& jAnimation = j.at("image_animation");

		std::string imageId;
		uint32_t numFramesRow = 1;
		uint32_t numFramesColumn = 1;
		uint32_t numAnimationFrames = 1;
		fromJson(imageId, jImage, "id", "");
		fromJson(numFramesRow, jImage, "frames_row", "");
		fromJson(numFramesColumn, jImage, "frames_column", "");
		fromJson(numAnimationFrames, jAnimation, "frames", "");

		if(numFramesRow > 1 || numFramesColumn > 1) {
			if(numAnimationFrames > 1) {
				sprite.shader = ShaderGraphBuilder::buildSpriteSheetAnimationShaderGraph(imageId, "",
					numFramesRow, numFramesColumn,
					jImage.contains("texture_origin") ? jImage.at("texture_origin").get<uint32_t>() : 0u,
					jImage.contains("frame") ? jImage.at("frame").get<uint32_t>() : 0u,
					numAnimationFrames,
					jAnimation.contains("duration") ? jAnimation.at("duration").get<floatd>() : 1.0,
					jAnimation.contains("loop") ? jAnimation.at("loop").get<bool>() : false,
					j.contains("color_mode") ? j.at("color_mode").get<uint32_t>() : 0u);
			}
			else {
				sprite.shader = ShaderGraphBuilder::buildSpriteSheetStaticShaderGraph(imageId, "",
					numFramesRow, numFramesColumn,
					jImage.contains("texture_origin") ? jImage.at("texture_origin").get<uint32_t>() : 0u,
					jImage.contains("frame") ? jImage.at("frame").get<uint32_t>() : 0u,
					jImage.contains("random_frame") ? jImage.at("random_frame").get<bool>() : false,
					j.contains("color_mode") ? j.at("color_mode").get<uint32_t>() : 0u);
			}
		}
		else {
			sprite.shader = ShaderGraphBuilder::buildTextureShaderGraph(imageId, "",
				j.contains("color_mode") ? j.at("color_mode").get<uint32_t>() : 0u);
		}

		if(jImage.contains("texture_pivot")) {
			sprite.pivot = jImage.at("texture_pivot");
		}
	}
}
}