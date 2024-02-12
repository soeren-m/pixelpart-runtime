#pragma once

#include "../common/VariantParameter.h"
#include <unordered_map>

namespace pixelpart {
struct MaterialInstance {
	std::string materialId;
	std::unordered_map<id_t, VariantParameter::Value> materialParameters;

	MaterialInstance();
	MaterialInstance(const std::string& matId, const std::vector<VariantParameter>& shaderParameters);
	MaterialInstance(const std::string& matId, const std::unordered_map<id_t, VariantParameter>& shaderParameters);
};

void to_json(nlohmann::ordered_json& j, const MaterialInstance& materialInstance);
void from_json(const nlohmann::ordered_json& j, MaterialInstance& materialInstance);
}