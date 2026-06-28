#include "BuiltInMaterialRepository.h"

namespace pixelpart {
BuiltInMaterialRepository::BuiltInMaterialRepository() {
	addMaterial("SpriteUnlitAlpha", ParticleRendererType::sprite, BlendMode::alpha, LightingMode::unlit, repositorySpriteUnlitParameterNames);
	addMaterial("SpriteUnlitAdditive", ParticleRendererType::sprite, BlendMode::additive, LightingMode::unlit, repositorySpriteUnlitParameterNames);
	addMaterial("TrailUnlitAlpha", ParticleRendererType::trail, BlendMode::alpha, LightingMode::unlit, repositoryTrailUnlitParameterNames);
	addMaterial("TrailUnlitAdditive", ParticleRendererType::trail, BlendMode::additive, LightingMode::unlit, repositoryTrailUnlitParameterNames);
	addMaterial("MeshUnlit", ParticleRendererType::mesh, BlendMode::off, LightingMode::unlit, repositoryMeshUnlitParameterNames);
	addMaterial("MeshUnlitAlpha", ParticleRendererType::mesh, BlendMode::alpha, LightingMode::unlit, repositoryMeshUnlitAlphaParameterNames);
	addMaterial("SpriteLitAlpha", ParticleRendererType::sprite, BlendMode::alpha, LightingMode::lit, repositorySpriteLitParameterNames);
	addMaterial("SpriteLitAdditive", ParticleRendererType::sprite, BlendMode::additive, LightingMode::lit, repositorySpriteLitParameterNames);
	addMaterial("TrailLitAlpha", ParticleRendererType::trail, BlendMode::alpha, LightingMode::lit, repositoryTrailLitParameterNames);
	addMaterial("TrailLitAdditive", ParticleRendererType::trail, BlendMode::additive, LightingMode::lit, repositoryTrailLitParameterNames);
	addMaterial("MeshLit", ParticleRendererType::mesh, BlendMode::off, LightingMode::lit, repositoryMeshLitParameterNames);
	addMaterial("MeshLitAlpha", ParticleRendererType::mesh, BlendMode::alpha, LightingMode::lit, repositoryMeshLitAlphaParameterNames);
}

const BuiltInMaterialMetadata* BuiltInMaterialRepository::material(const std::string& name) const {
	auto it = repositoryMaterials.find(name);
	if(it == repositoryMaterials.end()) {
		return nullptr;
	}

	return &(it->second);
}

const std::unordered_map<std::string, BuiltInMaterialMetadata>& BuiltInMaterialRepository::materials() const {
	return repositoryMaterials;
}

void BuiltInMaterialRepository::addMaterial(const std::string& name,
	ParticleRendererType remdererType, BlendMode blendMode, LightingMode lightingMode,
	const std::vector<std::string>& parameterNames) {
	BuiltInMaterialMetadata::ParameterCollection parameters;

	for(const std::string& parameterName : parameterNames) {
		id_t parameterId = repositoryMaterialParameterIds.at(parameterName);
		parameters[parameterId] = repositoryMaterialParameters.at(parameterName);
	}

	repositoryMaterials[name] = BuiltInMaterialMetadata(name, remdererType, blendMode, lightingMode, parameters);
}

const std::vector<std::string> BuiltInMaterialRepository::repositorySpriteUnlitParameterNames = std::vector<std::string>{
	"MainTexture",
	"Emission",
	"ColorBlendMode",
	"SpriteSheetRowNumber",
	"SpriteSheetColumnNumber",
	"SpriteSheetOrigin",
	"SpriteAnimationNumFrames",
	"SpriteAnimationStartFrame",
	"SpriteAnimationDuration",
	"SpriteAnimationLoop",
	"SoftParticles",
	"SoftParticleTransition",
	"DistanceFade",
	"DistanceFadeTransition"
};
const std::vector<std::string> BuiltInMaterialRepository::repositorySpriteLitParameterNames = std::vector<std::string>{
	"MainTexture",
	"Emission",
	"Roughness",
	"Metallic",
	"ColorBlendMode",
	"SpriteSheetRowNumber",
	"SpriteSheetColumnNumber",
	"SpriteSheetOrigin",
	"SpriteAnimationNumFrames",
	"SpriteAnimationStartFrame",
	"SpriteAnimationDuration",
	"SpriteAnimationLoop",
	"SoftParticles",
	"SoftParticleTransition",
	"DistanceFade",
	"DistanceFadeTransition"
};
const std::vector<std::string> BuiltInMaterialRepository::repositoryTrailUnlitParameterNames = std::vector<std::string>{
	"MainTexture",
	"Emission",
	"ColorBlendMode",
	"SoftParticles",
	"SoftParticleTransition",
	"DistanceFade",
	"DistanceFadeTransition"
};
const std::vector<std::string> BuiltInMaterialRepository::repositoryTrailLitParameterNames = std::vector<std::string>{
	"MainTexture",
	"Emission",
	"Roughness",
	"Metallic",
	"ColorBlendMode",
	"SoftParticles",
	"SoftParticleTransition",
	"DistanceFade",
	"DistanceFadeTransition"
};
const std::vector<std::string> BuiltInMaterialRepository::repositoryMeshUnlitParameterNames = std::vector<std::string>{
	"MainTexture",
	"Emission",
	"ColorBlendMode"
};
const std::vector<std::string> BuiltInMaterialRepository::repositoryMeshUnlitAlphaParameterNames = std::vector<std::string>{
	"MainTexture",
	"Emission",
	"ColorBlendMode",
	"SoftParticles",
	"SoftParticleTransition",
	"DistanceFade",
	"DistanceFadeTransition"
};
const std::vector<std::string> BuiltInMaterialRepository::repositoryMeshLitParameterNames = std::vector<std::string>{
	"MainTexture",
	"Emission",
	"Roughness",
	"Metallic",
	"ColorBlendMode"
};
const std::vector<std::string> BuiltInMaterialRepository::repositoryMeshLitAlphaParameterNames = std::vector<std::string>{
	"MainTexture",
	"Emission",
	"Roughness",
	"Metallic",
	"ColorBlendMode",
	"SoftParticles",
	"SoftParticleTransition",
	"DistanceFade",
	"DistanceFadeTransition"
};
const std::unordered_map<std::string, id_t> BuiltInMaterialRepository::repositoryMaterialParameterIds = std::unordered_map<std::string, id_t> {
	{ "MainTexture", 0 },
	{ "ColorBlendMode", 10 },
	{ "Emission", 20 },
	{ "Roughness", 21 },
	{ "Metallic", 22 },
	{ "SpriteSheetRowNumber", 30 },
	{ "SpriteSheetColumnNumber", 31 },
	{ "SpriteSheetOrigin", 32 },
	{ "SpriteAnimationNumFrames", 33 },
	{ "SpriteAnimationStartFrame", 34 },
	{ "SpriteAnimationDuration", 35 },
	{ "SpriteAnimationLoop", 36 },
	{ "SoftParticles", 40 },
	{ "SoftParticleTransition", 41 },
	{ "DistanceFade", 42 },
	{ "DistanceFadeTransition", 43 }
};
const std::unordered_map<std::string, VariantParameter> BuiltInMaterialRepository::repositoryMaterialParameters = std::unordered_map<std::string, VariantParameter>{
	{ "MainTexture", VariantParameter::ImageResourceParameter("MainTexture") },
	{ "ColorBlendMode", VariantParameter::EnumParameter("ColorBlendMode", 0, {
		"blend_multiply",
		"blend_add",
		"blend_subtract",
		"blend_difference",
		"blend_screen",
		"blend_overlay",
		"blend_lighten",
		"blend_darken",
		"blend_coloronly"
	}) },
	{ "Emission", VariantParameter::Float3Parameter("Emission", float3_t(0.0), float3_t(0.0), float3_t(1000.0)) },
	{ "Roughness", VariantParameter::FloatParameter("Roughness", 0.5, 0.0, 1.0) },
	{ "Metallic", VariantParameter::FloatParameter("Metallic", 0.0, 0.0, 1.0) },
	{ "SpriteSheetRowNumber", VariantParameter::IntParameter("SpriteSheetRowNumber", 1, 1, 1000) },
	{ "SpriteSheetColumnNumber", VariantParameter::IntParameter("SpriteSheetColumnNumber", 1, 1, 1000) },
	{ "SpriteSheetOrigin", VariantParameter::EnumParameter("SpriteSheetOrigin", 0, {
		"origin_bl",
		"origin_br",
		"origin_tl",
		"origin_tr"
	}) },
	{ "SpriteAnimationNumFrames", VariantParameter::IntParameter("SpriteAnimationNumFrames", 1, 1, 1000) },
	{ "SpriteAnimationStartFrame", VariantParameter::IntParameter("SpriteAnimationStartFrame", 0, 0, 1000) },
	{ "SpriteAnimationDuration", VariantParameter::FloatParameter("SpriteAnimationDuration", 1.0, 0.0, 1000.0) },
	{ "SpriteAnimationLoop", VariantParameter::BoolParameter("SpriteAnimationLoop", false) },
	{ "SoftParticles", VariantParameter::BoolParameter("SoftParticles", false) },
	{ "SoftParticleTransition", VariantParameter::FloatParameter("SoftParticleTransition", 0.1, 0.0, 1.0) },
	{ "DistanceFade", VariantParameter::BoolParameter("DistanceFade", false) },
	{ "DistanceFadeTransition", VariantParameter::FloatParameter("DistanceFadeTransition", 0.1, 0.0, 10.0) }
};
}
