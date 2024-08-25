#pragma once

#include "../common/VariantParameter.h"
#include "../json/json.hpp"
#include <string>
#include <unordered_map>

namespace pixelpart {
class MaterialInstance {
public:
	//MaterialInstance

private:
	bool isBuiltInMaterial = true;
	std::string materialId;
	std::unordered_map<id_t, VariantParameter::Value> materialParameters;
};

void to_json(nlohmann::ordered_json& j, const MaterialInstance& materialInstance);
void from_json(const nlohmann::ordered_json& j, MaterialInstance& materialInstance);
}