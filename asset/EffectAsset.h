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
class EffectAsset {
public:
	static const uint32_t version;

	EffectAsset() = default;

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

	bool usesName(const std::string& name) const;
	bool usesResource(const std::string& resourceId) const;

private:
	Effect assetEffect;

	ImageEffectSettings assetImageEffectSettings;
	CameraSettings assetCameraSettings;
	RenderSettings assetRenderSettings;
	RenderSettings assetPreviewSettings;
};

std::string serializeEffectAsset(const EffectAsset& asset, int32_t indent = 4);

EffectAsset deserializeEffectAsset(std::istream& stream);
EffectAsset deserializeEffectAsset(const std::string& data);
EffectAsset deserializeEffectAsset(const char* data, std::size_t size);

void to_json(nlohmann::ordered_json& j, const EffectAsset& asset);
void from_json(const nlohmann::ordered_json& j, EffectAsset& asset);
}