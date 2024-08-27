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
	const Effect& effect() const;

	ImageEffectSettings& imageEffectSettings();
	const ImageEffectSettings& imageEffectSettings() const;

	CameraSettings& cameraSettings();
	const CameraSettings& cameraSettings() const;

	RenderSettings& renderSettings();
	const RenderSettings& renderSettings() const;

	RenderSettings& previewSettings();
	const RenderSettings& previewSettings() const;

	bool isNameUsed(const std::string& name) const;
	bool isResourceUsed(const std::string& resourceId) const;

private:
	Effect projectEffect;

	ImageEffectSettings projectImageEffectSettings;
	CameraSettings projectCameraSettings;
	RenderSettings projectRenderSettings;
	RenderSettings projectPreviewSettings;
};

std::string serialize(const Project& project, int32_t indent = 4);

Project deserialize(std::istream& stream);
Project deserialize(const std::string& data);
Project deserialize(const char* data, std::size_t size);

void to_json(nlohmann::ordered_json& j, const Project& project);
void from_json(const nlohmann::ordered_json& j, Project& project);
}