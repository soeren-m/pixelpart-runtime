#include "MaterialResource.h"

namespace pixelpart {
MaterialResource::MaterialResource(const std::string& resName, BlendMode blend, LightingMode lighting, const ShaderGraph& graph) :
	Resource(resName), materialBlendMode(blend), materialLightingMode(lighting), materialShaderGraph(graph) {

}

void MaterialResource::blendMode(BlendMode mode) {
	materialBlendMode = mode;
}
BlendMode MaterialResource::blendMode() const {
	return materialBlendMode;
}

void MaterialResource::lightingMode(LightingMode mode) {
	materialLightingMode = mode;
}
LightingMode MaterialResource::lightingMode() const {
	return materialLightingMode;
}

ShaderGraph& MaterialResource::shaderGraph() {
	return materialShaderGraph;
}
const ShaderGraph& MaterialResource::shaderGraph() const {
	return materialShaderGraph;
}

void to_json(nlohmann::ordered_json& j, const MaterialResource& resource) {
	j = nlohmann::ordered_json{
		{ "name", resource.name() },
		{ "blend_mode", resource.blendMode() },
		{ "lighting_mode", resource.lightingMode() },
		{ "shader_graph", resource.shaderGraph() }
	};
}
void from_json(const nlohmann::ordered_json& j, MaterialResource& resource) {
	resource = MaterialResource(
		j.value("name", "unknown"),
		j.value("blend_mode", BlendMode::normal),
		j.value("lighting_mode", LightingMode::unlit),
		j.value("shader_graph", ShaderGraph()));
}
}