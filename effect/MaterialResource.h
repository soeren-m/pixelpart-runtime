#pragma once

#include "Resource.h"
#include "BlendMode.h"
#include "LightingMode.h"
#include "../shader/ShaderGraph.h"

namespace pixelpart {
struct MaterialResource : public Resource {
	BlendMode blendMode = BlendMode::normal;
	LightingMode lightingMode = LightingMode::unlit;
	ShaderGraph shaderGraph;
};

void to_json(nlohmann::ordered_json& j, const MaterialResource& resource);
void from_json(const nlohmann::ordered_json& j, MaterialResource& resource);
}