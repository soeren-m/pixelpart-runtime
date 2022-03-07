#pragma once

#include "Effect.h"
#include "PostProcessingEffect.h"
#include "RenderSettings.h"
#include "ResourceDatabase.h"

namespace pixelpart {
struct Project {
	static const uint32_t version;

	Effect effect;

	PostProcessingPipeline postProcessingEffects;
	vec4d backgroundColor = vec4d(0.0);
	vec2d cameraPosition = vec2d(0.0);
	floatd cameraZoom = 1.0;

	RenderSettings renderSettings;
	RenderSettings previewSettings;
};

void to_json(nlohmann::ordered_json& j, const Project& project);
void from_json(const nlohmann::ordered_json& j, Project& project);

std::string serialize(const Project& project, const ResourceDatabase& resources, int32_t indent = 4);

Project deserialize(std::istream& stream, ResourceDatabase& resources);
Project deserialize(const std::string& data, ResourceDatabase& resources);
Project deserialize(const char* data, std::size_t size, ResourceDatabase& resources);
}