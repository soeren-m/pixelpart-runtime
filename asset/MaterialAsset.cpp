#include "MaterialAsset.h"

namespace pixelpart {
const uint32_t MaterialAsset::version = 9;

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

std::string serializeMaterialAsset(const MaterialAsset& asset, int32_t indent) {
	nlohmann::ordered_json jsonData = asset;

	return jsonData.dump(indent);
}

MaterialAsset deserializeMaterialAsset(std::istream& stream) {
	nlohmann::ordered_json jsonData = nlohmann::ordered_json::parse(stream);

	return jsonData.get<MaterialAsset>();
}
MaterialAsset deserializeMaterialAsset(const std::string& data) {
	nlohmann::ordered_json jsonData = nlohmann::ordered_json::parse(data);

	return jsonData.get<MaterialAsset>();
}
MaterialAsset deserializeMaterialAsset(const char* data, std::size_t size) {
	nlohmann::ordered_json jsonData = nlohmann::ordered_json::parse(data, data + size);

	return jsonData.get<MaterialAsset>();
}

void to_json(nlohmann::ordered_json& j, const MaterialAsset& asset) {
	j = nlohmann::ordered_json{
		{ "version", MaterialAsset::version },
		{ "material", asset.material() },
		{ "images", asset.images() }
	};
}
void from_json(const nlohmann::ordered_json& j, MaterialAsset& asset) {
	uint32_t version = j.at("version");
	if(version != MaterialAsset::version) {
		throw std::runtime_error("Unsupported material asset version " + std::to_string(version));
	}

	asset.material() = j.value("material", MaterialResource());
	asset.images() = j.value("images", std::unordered_map<std::string, ImageResource>());
}
}