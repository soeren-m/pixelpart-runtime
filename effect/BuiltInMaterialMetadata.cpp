#include "BuiltInMaterialMetadata.h"

namespace pixelpart {
BuiltInMaterialMetadata::BuiltInMaterialMetadata(const std::string& name,
    ParticleRendererType rendererType,
    BlendMode blendMode,
	LightingMode lightingMode,
    const ParameterCollection& parameters) :
	materialName(name), materialRendererType(rendererType),
	materialBlendMode(blendMode), materialLightingMode(lightingMode),
	materialParameters(parameters) {

}

const std::string& BuiltInMaterialMetadata::name() const {
	return materialName;
}

ParticleRendererType BuiltInMaterialMetadata::rendererType() const {
	return materialRendererType;
}
BlendMode BuiltInMaterialMetadata::blendMode() const {
	return materialBlendMode;
}
LightingMode BuiltInMaterialMetadata::lightingMode() const {
	return materialLightingMode;
}

const BuiltInMaterialMetadata::ParameterCollection& BuiltInMaterialMetadata::parameters() const {
	return materialParameters;
}
}
