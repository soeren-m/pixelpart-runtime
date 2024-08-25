#pragma once

#include "../common/Types.h"
#include "../effect/Effect.h"
#include "ImageEffectSettings.h"
#include "CameraSettings.h"
#include "RenderSettings.h"
#include "../json/json.hpp"
#include <string>
#include <istream>

namespace pixelpart {
class Project {
public:
	static const uint32_t version;

	Project() = default;

	Effect& effect();

	// TODO

	bool isNameUsed(const std::string& name) const;
	bool isResourceUsed(const std::string& resourceId) const;

private:
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