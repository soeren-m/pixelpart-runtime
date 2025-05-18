#include "MaterialAsset.h"
#include "../common/SerializationException.h"
#include "../common/DeserializationException.h"

namespace pixelpart {
const std::uint32_t MaterialAsset::version = 9;

MaterialResource& MaterialAsset::material() {
	return assetMaterial;
}
const MaterialResource& MaterialAsset::material() const {
	return assetMaterial;
}

std::unordered_map<std::string, ImageResource>& MaterialAsset::images() {
	return assetImages;
}
const std::unordered_map<std::string, ImageResource>& MaterialAsset::images() const {
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

		return jsonData.get<MaterialAsset>();
	}
	catch(const nlohmann::ordered_json::exception& e) {
		throw DeserializationException(e.what());
	}
}
MaterialAsset deserializeMaterialAsset(const std::string& data) {
	try {
		nlohmann::ordered_json jsonData = nlohmann::ordered_json::parse(data);

		return jsonData.get<MaterialAsset>();
	}
	catch(const nlohmann::ordered_json::exception& e) {
		throw DeserializationException(e.what());
	}
}
MaterialAsset deserializeMaterialAsset(const char* data, std::size_t size) {
	try {
		nlohmann::ordered_json jsonData = nlohmann::ordered_json::parse(data, data + size);

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
		{ "images", asset.images() }
	};
}
void from_json(const nlohmann::ordered_json& j, MaterialAsset& asset) {
	std::uint32_t version = j.at("version");
	if(version != MaterialAsset::version) {
		throw DeserializationException("Unsupported material asset version " + std::to_string(version));
	}

	asset.material() = j.value("material", MaterialResource());
	asset.images() = j.value("images", std::unordered_map<std::string, ImageResource>());
}
}