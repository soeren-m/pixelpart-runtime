#include "Sprite.h"
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
		{ "image", sprite.image },
		{ "image_animation", sprite.imageAnimation },
		{ "color", sprite.color },
		{ "opacity", sprite.opacity },
		{ "color_mode", sprite.colorMode },
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
	fromJson(sprite.image, j, "image", "");
	fromJson(sprite.imageAnimation, j, "image_animation", "");
	fromJson(sprite.color, j, "color", "");
	fromJson(sprite.opacity, j, "opacity", "");
	fromJson(sprite.colorMode, j, "color_mode", "");
	fromJson(sprite.blendMode, j, "blend_mode", "");
	fromJson(sprite.layer, j, "layer", "");
	fromJson(sprite.visible, j, "visible", "");
}
}