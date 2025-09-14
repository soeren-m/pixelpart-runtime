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
	using ParameterValueMap = std::unordered_map<id_t, VariantParameter::Value>;

	MaterialInstance() = default;
	MaterialInstance(const std::string& materialId, bool builtIn);
	MaterialInstance(const std::string& materialId, bool builtIn, const ParameterValueMap& parameters);
	MaterialInstance(const std::string& materialId, const MaterialResource& material);

	const std::string& materialId() const;

	bool builtInMaterial() const;

	ParameterValueMap& materialParameters();
	const ParameterValueMap& materialParameters() const;

private:
	std::string instanceMaterialId;
	bool instanceBuiltInMaterial = true;

	ParameterValueMap instanceMaterialParameters;
};

void to_json(nlohmann::ordered_json& j, const MaterialInstance& materialInstance);
void from_json(const nlohmann::ordered_json& j, MaterialInstance& materialInstance);
}
