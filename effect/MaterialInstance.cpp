#include "MaterialInstance.h"

namespace pixelpart {
MaterialInstance::MaterialInstance(const std::string& materialId, bool builtIn) :
	instanceMaterialId(materialId), instanceBuiltInMaterial(builtIn) {

}
MaterialInstance::MaterialInstance(const std::string& materialId, bool builtIn, const std::unordered_map<id_t, VariantParameter::Value>& parameters) :
	instanceMaterialId(materialId), instanceBuiltInMaterial(builtIn), instanceMaterialParameters(parameters) {

}
MaterialInstance::MaterialInstance(const std::string& materialId, const MaterialResource& material) :
	instanceMaterialId(materialId), instanceBuiltInMaterial(false) {
	for(const auto& parameterEntry : material.shaderGraph().shaderParameters()) {
		const pixelpart::VariantParameter& parameter = parameterEntry.second;

		if(instanceMaterialParameters.count(parameterEntry.first) == 0) {
			pixelpart::VariantParameter::Value initialValue = parameter.def();
			if(material.shaderGraph().containsNode(parameterEntry.first)) {
				initialValue = material.shaderGraph().node(parameterEntry.first).parameters().at(0);
			}

			instanceMaterialParameters[parameterEntry.first] = initialValue;
		}
	}
}

const std::string& MaterialInstance::materialId() const {
	return instanceMaterialId;
}

bool MaterialInstance::builtInMaterial() const {
	return instanceBuiltInMaterial;
}

std::unordered_map<id_t, VariantParameter::Value>& MaterialInstance::materialParameters() {
	return instanceMaterialParameters;
}
const std::unordered_map<id_t, VariantParameter::Value>& MaterialInstance::materialParameters() const {
	return instanceMaterialParameters;
}

void to_json(nlohmann::ordered_json& j, const MaterialInstance& materialInstance) {
	j = nlohmann::ordered_json{
		{ "builtin_material", materialInstance.builtInMaterial() },
		{ "material_id", materialInstance.materialId() },
		{ "material_parameters", materialInstance.materialParameters() }
	};
}
void from_json(const nlohmann::ordered_json& j, MaterialInstance& materialInstance) {
	materialInstance = MaterialInstance(
		j.at("material_id"),
		j.value("builtin_material", true),
		j.value("material_parameters", std::unordered_map<id_t, VariantParameter::Value>()));
}
}
