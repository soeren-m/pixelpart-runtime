#include "ImageAnimation.h"
#include "JSONUtil.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const ImageAnimation& animation) {
	j = nlohmann::ordered_json{
		{ "frames", animation.frames },
		{ "duration", animation.duration },
		{ "loop", animation.loop }
	};
}
void from_json(const nlohmann::ordered_json& j, ImageAnimation& animation) {
	animation = ImageAnimation();
	fromJson(animation.frames, j, "frames", "");
	fromJson(animation.duration, j, "duration", "");
	fromJson(animation.loop, j, "loop", "");
}
}