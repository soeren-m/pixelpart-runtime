#include "EffectAsset.h"
#include "../common/SerializationException.h"
#include "../common/DeserializationException.h"
#include "../effect/Node.h"
#include "../effect/ParticleType.h"
#include <memory>

namespace pixelpart {
const uint32_t EffectAsset::version = 9u;

Effect& EffectAsset::effect() {
	return assetEffect;
}
const Effect& EffectAsset::effect() const {
	return assetEffect;
}

ImageEffectSettings& EffectAsset::imageEffectSettings() {
	return assetImageEffectSettings;
}
const ImageEffectSettings& EffectAsset::imageEffectSettings() const {
	return assetImageEffectSettings;
}

CameraSettings& EffectAsset::cameraSettings() {
	return assetCameraSettings;
}
const CameraSettings& EffectAsset::cameraSettings() const {
	return assetCameraSettings;
}

RenderSettings& EffectAsset::renderSettings() {
	return assetRenderSettings;
}
const RenderSettings& EffectAsset::renderSettings() const {
	return assetRenderSettings;
}

RenderSettings& EffectAsset::previewSettings() {
	return assetPreviewSettings;
}
const RenderSettings& EffectAsset::previewSettings() const {
	return assetPreviewSettings;
}

bool EffectAsset::usesName(const std::string& name) const {
	for(const std::unique_ptr<Node>& node : assetEffect.sceneGraph()) {
		if(node->name() == name) {
			return true;
		}
	}

	for(const ParticleType& particleType : assetEffect.particleTypes()) {
		if(particleType.name() == name) {
			return true;
		}
	}

	return false;
}
bool EffectAsset::usesResource(const std::string& resourceId) const {
	for(const std::unique_ptr<Node>& node : assetEffect.sceneGraph()) {
		if(node->usesResource(resourceId)) {
			return true;
		}
	}

	for(const ParticleType& particleType : assetEffect.particleTypes()) {
		if(particleType.usesResource(resourceId)) {
			return true;
		}
	}

	for(const auto& materialResourceEntry : assetEffect.resources().materials()) {
		const MaterialResource& material = materialResourceEntry.second;

		for(const auto& nodeEntry : material.shaderGraph().nodes()) {
			for(const auto& nodeParameter : nodeEntry.second.parameters()) {
				if(nodeParameter.type() == pixelpart::VariantParameter::Value::type_resource_image &&
					resourceId == nodeParameter.valueResourceId()) {
					return true;
				}
			}
		}
	}

	return false;
}

std::string serializeEffectAsset(const EffectAsset& asset, int32_t indent) {
	try {
		nlohmann::ordered_json jsonData = asset;

		return jsonData.dump(indent);
	}
	catch(const nlohmann::ordered_json::exception& e) {
		throw SerializationException(e.what());
	}
}

EffectAsset deserializeEffectAsset(std::istream& stream) {
	try {
		nlohmann::ordered_json jsonData = nlohmann::ordered_json::parse(stream);

		return jsonData.get<EffectAsset>();
	}
	catch(const nlohmann::ordered_json::exception& e) {
		throw DeserializationException(e.what());
	}
}
EffectAsset deserializeEffectAsset(const std::string& data) {
	try {
		nlohmann::ordered_json jsonData = nlohmann::ordered_json::parse(data);

		return jsonData.get<EffectAsset>();
	}
	catch(const nlohmann::ordered_json::exception& e) {
		throw DeserializationException(e.what());
	}
}
EffectAsset deserializeEffectAsset(const char* data, std::size_t size) {
	try {
		nlohmann::ordered_json jsonData = nlohmann::ordered_json::parse(data, data + size);

		return jsonData.get<EffectAsset>();
	}
	catch(const nlohmann::ordered_json::exception& e) {
		throw DeserializationException(e.what());
	}
}

void to_json(nlohmann::ordered_json& j, const EffectAsset& asset) {
	j = nlohmann::ordered_json{
		{ "version", EffectAsset::version },
		{ "effect", asset.effect() },
		{ "image_effects", asset.imageEffectSettings() },
		{ "camera", asset.cameraSettings() },
		{ "render", asset.renderSettings() },
		{ "preview", asset.previewSettings() },
	};
}
void from_json(const nlohmann::ordered_json& j, EffectAsset& asset) {
	uint32_t version = j.at("version");
	if(version != EffectAsset::version) {
		throw DeserializationException("Unsupported effect asset version " + std::to_string(version));
	}

	asset.effect() = j.value("effect", Effect());

	for(const std::unique_ptr<Node>& node : asset.effect().sceneGraph()) {
		if(!node->name().empty()) {
			continue;
		}

		uint32_t counter = 1u;
		std::string name = "Node";
		while(asset.usesName(name)) {
			name = "Node" + std::to_string(counter++);
		}

		node->name(name);
	}

	for(ParticleType& particleType : asset.effect().particleTypes()) {
		if(!particleType.name().empty()) {
			continue;
		}

		uint32_t counter = 1u;
		std::string name = "Particle";
		while(asset.usesName(name)) {
			name = "Particle" + std::to_string(counter++);
		}

		particleType.name(name);
	}

	asset.imageEffectSettings() = j.value("image_effects", ImageEffectSettings());
	asset.cameraSettings() = j.value("camera", CameraSettings());
	asset.renderSettings() = j.value("render", RenderSettings());
	asset.previewSettings() = j.value("preview", RenderSettings());
}
}