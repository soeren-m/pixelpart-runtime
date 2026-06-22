#include "MaterialAsset.h"
#include "../common/Serialization.h"
#include "../common/SortedJson.h"

namespace pixelpart {
namespace {
void migrateMaterialAssetJson(nlohmann::ordered_json& jsonData) {
	std::uint32_t version = jsonData.at("version");
	if(version == MaterialAsset::version) {
		return;
	}
	else if(version < 8) {
		throw DeserializationException("Unsupported material asset version " + std::to_string(version));
	}

	// Migration 1.7.x -> 1.8.x
	if(version == 8) {
		for(nlohmann::ordered_json& jImage : jsonData["images"]) {
			jImage["color_space"] = "srgb";
			jImage["channels"] = jImage.value("bpp", 0u) / 8;
			jImage.erase("bpp");
		}

		version = 9;
		jsonData["version"] = 9;
	}

	// Migration 1.8.x -> 1.9.x
	if(version == 9) {
		nlohmann::ordered_json& jMaterial = jsonData["material"];
		std::string blendMode = jMaterial["blend_mode"];
		if(blendMode == "normal") {
			jMaterial["blend_mode"] = "alpha";
		}

		version = 10;
		jsonData["version"] = 10;
	}

	// Migration 1.9.x -> 1.10.x
	if(version == 10) {
		version = 11;
		jsonData["version"] = 11;
	}
}
}

const std::uint32_t MaterialAsset::version = 11;

MaterialResource& MaterialAsset::material() {
	return assetMaterial;
}
const MaterialResource& MaterialAsset::material() const {
	return assetMaterial;
}

ResourceMap<ImageResource>& MaterialAsset::images() {
	return assetImages;
}
const ResourceMap<ImageResource>& MaterialAsset::images() const {
	return assetImages;
}

std::string serializeMaterialAsset(const MaterialAsset& asset, std::int32_t indent) {
	try {
		nlohmann::ordered_json jsonData = asset;

		return jsonData.dump(indent);
	}
	catch(const nlohmann::ordered_json::exception& e) {
		throw SerializationException(e.what());
	}
}

MaterialAsset deserializeMaterialAsset(std::istream& stream) {
	try {
		nlohmann::ordered_json jsonData = nlohmann::ordered_json::parse(stream);
		migrateMaterialAssetJson(jsonData);

		return jsonData.get<MaterialAsset>();
	}
	catch(const nlohmann::ordered_json::exception& e) {
		throw DeserializationException(e.what());
	}
}
MaterialAsset deserializeMaterialAsset(const std::string& data) {
	try {
		nlohmann::ordered_json jsonData = nlohmann::ordered_json::parse(data);
		migrateMaterialAssetJson(jsonData);

		return jsonData.get<MaterialAsset>();
	}
	catch(const nlohmann::ordered_json::exception& e) {
		throw DeserializationException(e.what());
	}
}
MaterialAsset deserializeMaterialAsset(const char* data, std::size_t size) {
	try {
		nlohmann::ordered_json jsonData = nlohmann::ordered_json::parse(data, data + size);
		migrateMaterialAssetJson(jsonData);

		return jsonData.get<MaterialAsset>();
	}
	catch(const nlohmann::ordered_json::exception& e) {
		throw DeserializationException(e.what());
	}
}

void to_json(nlohmann::ordered_json& j, const MaterialAsset& asset) {
	j = nlohmann::ordered_json{
		{ "version", MaterialAsset::version },
		{ "material", asset.material() },
		{ "images", toSortedJson(asset.images()) }
	};
}
void from_json(const nlohmann::ordered_json& j, MaterialAsset& asset) {
	std::uint32_t version = j.at("version");
	if(version != MaterialAsset::version) {
		throw DeserializationException("Unsupported material asset version " + std::to_string(version));
	}

	asset.material() = j.value("material", MaterialResource());
	asset.images() = j.value("images", ResourceMap<ImageResource>());
}
}
