#pragma once

#include "../effect/Effect.h"
#include "ImageEffectSettings.h"
#include "CameraSettings.h"
#include "RenderSettings.h"

namespace pixelpart {
struct Project {
	static const uint32_t version;

	Effect effect;

	ImageEffectSettings imageEffectSettings;
	CameraSettings cameraSettings;
	RenderSettings renderSettings;
	RenderSettings previewSettings;
};

void to_json(nlohmann::ordered_json& j, const Project& project);
void from_json(const nlohmann::ordered_json& j, Project& project);

std::string serialize(const Project& project, int32_t indent = 4);

Project deserialize(std::istream& stream);
Project deserialize(const std::string& data);
Project deserialize(const char* data, std::size_t size);
}