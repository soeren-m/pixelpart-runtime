#include "MaterialInstance.h"
#include "../common/Json.h"

namespace pixelpart {
MaterialInstance::MaterialInstance() {

}
MaterialInstance::MaterialInstance(const std::string& matId, const std::vector<VariantParameter>& shaderParameters) : materialId(matId) {
	uint32_t index = 0u;
	for(const VariantParameter& parameter : shaderParameters) {
		materialParameters[index++] = parameter.defaultValue;
	}
}
MaterialInstance::MaterialInstance(const std::string& matId, const std::unordered_map<id_t, VariantParameter>& shaderParameters) : materialId(matId) {
	for(const auto& parameterEntry : shaderParameters) {
		materialParameters[parameterEntry.first] = parameterEntry.second.defaultValue;
	}
}

void to_json(nlohmann::ordered_json& j, const MaterialInstance& materialInstance) {
	j = nlohmann::ordered_json{
		{ "material_id", materialInstance.materialId },
		{ "material_parameters", materialInstance.materialParameters }
	};
}
void from_json(const nlohmann::ordered_json& j, MaterialInstance& materialInstance) {
	materialInstance = MaterialInstance();

	fromJson(materialInstance.materialId, j, "material_id");
	fromJson(materialInstance.materialParameters, j, "material_parameters");
}
}