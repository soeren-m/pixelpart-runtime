#pragma once

#include "../common/VariantParameter.h"
#include "MaterialResource.h"
#include "../json/json.hpp"
#include <string>
#include <unordered_map>

namespace pixelpart {
class MaterialInstance {
public:
	MaterialInstance() = default;
	MaterialInstance(const std::string& materialId, bool builtIn, const std::unordered_map<id_t, VariantParameter::Value>& parameters);
	MaterialInstance(const MaterialResource& material);

	const std::string& materialId() const;
	bool builtInMaterial() const;

	const std::unordered_map<id_t, VariantParameter::Value>& materialParameters() const;

private:
	std::string instanceMaterialId;
	bool instanceBuiltInMaterial = true;

	std::unordered_map<id_t, VariantParameter::Value> instanceMaterialParameters;
};

void to_json(nlohmann::ordered_json& j, const MaterialInstance& materialInstance);
void from_json(const nlohmann::ordered_json& j, MaterialInstance& materialInstance);
}