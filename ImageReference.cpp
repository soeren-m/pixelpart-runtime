#include "ImageReference.h"
#include "JSONUtil.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const ImageReference& img) {
	j = nlohmann::ordered_json{
		{ "id", img.id },
		{ "texture_origin", img.textureOrigin },
		{ "texture_pivot", img.texturePivot },
		{ "frames_row", img.framesRow },
		{ "frames_column", img.framesColumn },
		{ "frame", img.frame },
		{ "random_frame", img.randomFrame },
	};
}
void from_json(const nlohmann::ordered_json& j, ImageReference& img) {
	img = ImageReference();
	fromJson(img.id, j, "id", "");
	fromJson(img.textureOrigin, j, "texture_origin", "");
	fromJson(img.texturePivot, j, "texture_pivot", "");
	fromJson(img.framesRow, j, "frames_row", "");
	fromJson(img.framesColumn, j, "frames_column", "");
	fromJson(img.frame, j, "frame", "");
	fromJson(img.randomFrame, j, "random_frame", "");
}
}