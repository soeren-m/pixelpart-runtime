#pragma once

#include "BuiltInMaterialMetadata.h"
#include "ParticleRendererType.h"
#include "BlendMode.h"
#include "LightingMode.h"
#include "VariantParameter.h"
#include "../types/Id.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace pixelpart {
class BuiltInMaterialRepository {
public:
	BuiltInMaterialRepository();

	const BuiltInMaterialMetadata* material(const std::string& name) const;

	const std::unordered_map<std::string, BuiltInMaterialMetadata>& materials() const;

private:
	void addMaterial(const std::string& name,
		ParticleRendererType rendererType, BlendMode blendMode, LightingMode lightingMode,
		const std::vector<std::string>& parameterNames,
		const std::unordered_map<std::string, id_t>& parameterIdMap,
		const std::unordered_map<std::string, VariantParameter>& parameterMap);

	std::unordered_map<std::string, BuiltInMaterialMetadata> repositoryMaterials;
};
}
