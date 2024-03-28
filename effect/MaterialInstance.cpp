#include "MaterialInstance.h"
#include "../common/Json.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const MaterialInstance& materialInstance) {
	j = nlohmann::ordered_json{
		{ "builtin_material", materialInstance.isBuiltInMaterial },
		{ "material_id", materialInstance.materialId },
		{ "material_parameters", materialInstance.materialParameters }
	};
}
void from_json(const nlohmann::ordered_json& j, MaterialInstance& materialInstance) {
	materialInstance = MaterialInstance();

	fromJson(materialInstance.isBuiltInMaterial, j, "builtin_material");
	fromJson(materialInstance.materialId, j, "material_id");
	fromJson(materialInstance.materialParameters, j, "material_parameters");
}
}