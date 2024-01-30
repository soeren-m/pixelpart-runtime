#include "EffectResourceSet.h"
#include "../common/JsonUtil.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const EffectResourceSet& resources) {
	j = nlohmann::ordered_json{
		{ "images", resources.images },
		{ "meshes", resources.meshes },
		{ "materials", resources.materials },
		{ "vector_fields", resources.vectorFields }
	};
}
void from_json(const nlohmann::ordered_json& j, EffectResourceSet& resources) {
	resources = EffectResourceSet();

	fromJson(resources.images, j, "images");
	fromJson(resources.meshes, j, "meshes");
	fromJson(resources.materials, j, "materials");
	fromJson(resources.vectorFields, j, "vector_fields");
}
}