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
		ParticleRendererType remdererType, BlendMode blendMode, LightingMode lightingMode,
		const std::vector<std::string>& parameterNames);

	std::unordered_map<std::string, BuiltInMaterialMetadata> repositoryMaterials;

	static const std::vector<std::string> repositorySpriteUnlitParameterNames;
	static const std::vector<std::string> repositorySpriteLitParameterNames;
	static const std::vector<std::string> repositoryTrailUnlitParameterNames;
	static const std::vector<std::string> repositoryTrailLitParameterNames;
	static const std::vector<std::string> repositoryMeshUnlitParameterNames;
	static const std::vector<std::string> repositoryMeshUnlitAlphaParameterNames;
	static const std::vector<std::string> repositoryMeshLitParameterNames;
	static const std::vector<std::string> repositoryMeshLitAlphaParameterNames;
	static const std::unordered_map<std::string, id_t> repositoryMaterialParameterIds;
	static const std::unordered_map<std::string, VariantParameter> repositoryMaterialParameters;
};
}
