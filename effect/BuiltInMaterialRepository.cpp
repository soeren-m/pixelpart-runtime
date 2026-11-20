#include "BuiltInMaterialRepository.h"

namespace pixelpart {
template<typename T, typename... Vectors>
std::vector<T> concat(const std::vector<T>& first, const Vectors&... rest) {
	std::vector<T> result;
	result.reserve(first.size() + (rest.size() + ...));
	result.insert(result.end(), first.begin(), first.end());
	(result.insert(result.end(), rest.begin(), rest.end()), ...);

	return result;
}

BuiltInMaterialRepository::BuiltInMaterialRepository() {
	const std::vector<std::string> spriteParameters = std::vector<std::string>{
		"MainTexture",
		"Emission",
		"ColorBlendMode",
		"SpriteSheetRowNumber",
		"SpriteSheetColumnNumber",
		"SpriteSheetOrigin",
		"SpriteAnimationNumFrames",
		"SpriteAnimationStartFrame",
		"SpriteAnimationDuration",
		"SpriteAnimationLoop"
	};
	const std::vector<std::string> trailParameters = std::vector<std::string>{
		"MainTexture",
		"Emission",
		"ColorBlendMode"
	};
	const std::vector<std::string> meshParameters = std::vector<std::string>{
		"MainTexture",
		"Emission",
		"ColorBlendMode"
	};

	const std::vector<std::string> opaqueParameters = std::vector<std::string>{
		"AlphaThreshold"
	};
	const std::vector<std::string> transparentParameters = std::vector<std::string>{
		"SoftParticles",
		"SoftParticleTransition",
		"DistanceFade",
		"DistanceFadeTransition"
	};

	const std::vector<std::string> litParameters = std::vector<std::string>{
		"Roughness",
		"Metallic"
	};

	const std::unordered_map<std::string, id_t> materialParameterIds = std::unordered_map<std::string, id_t> {
		{ "MainTexture", 0 },
		{ "ColorBlendMode", 10 },
		{ "AlphaThreshold", 11 },
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

	const std::unordered_map<std::string, VariantParameter> materialParameterInfo = std::unordered_map<std::string, VariantParameter>{
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
		{ "AlphaThreshold", VariantParameter::FloatParameter("AlphaThreshold", 0.5, 0.0, 1.0) },
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

	addMaterial("SpriteUnlitOpaque", ParticleRendererType::sprite, BlendMode::off, LightingMode::unlit, concat(spriteParameters, opaqueParameters), materialParameterIds, materialParameterInfo);
	addMaterial("SpriteUnlitAlpha", ParticleRendererType::sprite, BlendMode::alpha, LightingMode::unlit, concat(spriteParameters, transparentParameters), materialParameterIds, materialParameterInfo);
	addMaterial("SpriteUnlitAdditive", ParticleRendererType::sprite, BlendMode::additive, LightingMode::unlit, concat(spriteParameters, transparentParameters), materialParameterIds, materialParameterInfo);
	addMaterial("SpriteLitOpaque", ParticleRendererType::sprite, BlendMode::off, LightingMode::lit, concat(spriteParameters, opaqueParameters, litParameters), materialParameterIds, materialParameterInfo);
	addMaterial("SpriteLitAlpha", ParticleRendererType::sprite, BlendMode::alpha, LightingMode::lit, concat(spriteParameters, transparentParameters, litParameters), materialParameterIds, materialParameterInfo);
	addMaterial("SpriteLitAdditive", ParticleRendererType::sprite, BlendMode::additive, LightingMode::lit, concat(spriteParameters, transparentParameters, litParameters), materialParameterIds, materialParameterInfo);

	addMaterial("TrailUnlitOpaque", ParticleRendererType::trail, BlendMode::off, LightingMode::unlit, concat(trailParameters, opaqueParameters), materialParameterIds, materialParameterInfo);
	addMaterial("TrailUnlitAlpha", ParticleRendererType::trail, BlendMode::alpha, LightingMode::unlit, concat(trailParameters, transparentParameters), materialParameterIds, materialParameterInfo);
	addMaterial("TrailUnlitAdditive", ParticleRendererType::trail, BlendMode::additive, LightingMode::unlit, concat(trailParameters, transparentParameters), materialParameterIds, materialParameterInfo);
	addMaterial("TrailLitOpaque", ParticleRendererType::trail, BlendMode::off, LightingMode::lit, concat(trailParameters, opaqueParameters, litParameters), materialParameterIds, materialParameterInfo);
	addMaterial("TrailLitAlpha", ParticleRendererType::trail, BlendMode::alpha, LightingMode::lit, concat(trailParameters, transparentParameters, litParameters), materialParameterIds, materialParameterInfo);
	addMaterial("TrailLitAdditive", ParticleRendererType::trail, BlendMode::additive, LightingMode::lit, concat(trailParameters, transparentParameters, litParameters), materialParameterIds, materialParameterInfo);

	addMaterial("MeshUnlitOpaque", ParticleRendererType::mesh, BlendMode::off, LightingMode::unlit, concat(meshParameters, opaqueParameters), materialParameterIds, materialParameterInfo);
	addMaterial("MeshUnlitAlpha", ParticleRendererType::mesh, BlendMode::alpha, LightingMode::unlit, concat(meshParameters, transparentParameters), materialParameterIds, materialParameterInfo);
	addMaterial("MeshLitOpaque", ParticleRendererType::mesh, BlendMode::off, LightingMode::lit, concat(meshParameters, opaqueParameters, litParameters), materialParameterIds, materialParameterInfo);
	addMaterial("MeshLitAlpha", ParticleRendererType::mesh, BlendMode::alpha, LightingMode::lit, concat(meshParameters, transparentParameters, litParameters), materialParameterIds, materialParameterInfo);
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
	ParticleRendererType rendererType, BlendMode blendMode, LightingMode lightingMode,
	const std::vector<std::string>& parameterNames,
	const std::unordered_map<std::string, id_t>& parameterIdMap,
	const std::unordered_map<std::string, VariantParameter>& parameterMap) {
	BuiltInMaterialMetadata::ParameterCollection parameters;

	for(const std::string& parameterName : parameterNames) {
		id_t parameterId = parameterIdMap.at(parameterName);
		parameters[parameterId] = parameterMap.at(parameterName);
	}

	repositoryMaterials[name] = BuiltInMaterialMetadata(name, rendererType, blendMode, lightingMode, parameters);
}
}
