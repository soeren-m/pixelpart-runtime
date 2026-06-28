#pragma once

#include "ParticleRendererType.h"
#include "BlendMode.h"
#include "LightingMode.h"
#include "VariantParameter.h"
#include "../types/Id.h"
#include <string>
#include <unordered_map>

namespace pixelpart {
class BuiltInMaterialMetadata {
public:
	using ParameterCollection = std::unordered_map<id_t, VariantParameter>;

	BuiltInMaterialMetadata() = default;
	BuiltInMaterialMetadata(const std::string& name,
		ParticleRendererType rendererType,
		BlendMode blendMode,
		LightingMode lightingMode,
		const ParameterCollection& parameters);

	const std::string& name() const;

	ParticleRendererType rendererType() const;
	BlendMode blendMode() const;
	LightingMode lightingMode() const;

	const ParameterCollection& parameters() const;

private:
	std::string materialName;

	ParticleRendererType materialRendererType;
	BlendMode materialBlendMode;
	LightingMode materialLightingMode;

	ParameterCollection materialParameters;
};
}
