#pragma once

#include "MaterialResource.h"
#include "../common/VariantParameter.h"
#include "../common/Id.h"
#include "../json/json.hpp"
#include <string>
#include <unordered_map>

namespace pixelpart {
class MaterialInstance {
public:
	MaterialInstance() = default;
	MaterialInstance(const std::string& materialId, bool builtIn);
	MaterialInstance(const std::string& materialId, bool builtIn, const std::unordered_map<id_t, VariantParameter::Value>& parameters);
	MaterialInstance(const std::string& materialId, const MaterialResource& material);

	const std::string& materialId() const;

	bool builtInMaterial() const;

	std::unordered_map<id_t, VariantParameter::Value>& materialParameters();
	const std::unordered_map<id_t, VariantParameter::Value>& materialParameters() const;

private:
	std::string instanceMaterialId;
	bool instanceBuiltInMaterial = true;

	std::unordered_map<id_t, VariantParameter::Value> instanceMaterialParameters;
};

void to_json(nlohmann::ordered_json& j, const MaterialInstance& materialInstance);
void from_json(const nlohmann::ordered_json& j, MaterialInstance& materialInstance);
}