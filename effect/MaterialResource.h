#pragma once

#include "Resource.h"
#include "BlendMode.h"
#include "LightingMode.h"
#include "../shadergraph/ShaderGraph.h"
#include "../json/json.hpp"

namespace pixelpart {
class MaterialResource : public Resource {
public:
	MaterialResource() = default;
	MaterialResource(const std::string& resName, BlendMode blend, LightingMode lighting, const ShaderGraph& graph);

	void blendMode(BlendMode mode);
	BlendMode blendMode() const;

	void lightingMode(LightingMode mode);
	LightingMode lightingMode() const;

	ShaderGraph& shaderGraph();
	const ShaderGraph& shaderGraph() const;

private:
	BlendMode materialBlendMode = BlendMode::normal;
	LightingMode materialLightingMode = LightingMode::unlit;
	ShaderGraph materialShaderGraph;
};

void to_json(nlohmann::ordered_json& j, const MaterialResource& resource);
void from_json(const nlohmann::ordered_json& j, MaterialResource& resource);
}