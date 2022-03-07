#include "ResourceDatabase.h"
#include "JSONUtil.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const ResourceDatabase& resources) {
	j = nlohmann::ordered_json{
		{ "images", resources.images }
	};
}
void from_json(const nlohmann::ordered_json& j, ResourceDatabase& resources) {
	resources = ResourceDatabase();
	fromJson(resources.images, j, "images", "");
}
}