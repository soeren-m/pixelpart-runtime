#pragma once

#include "../common/Types.h"
#include "../effect/MaterialResource.h"
#include "../effect/ImageResource.h"
#include "../json/json.hpp"
#include <string>
#include <unordered_map>
#include <istream>

namespace pixelpart {
class MaterialAsset {
public:
	static const std::uint32_t version;

	MaterialAsset() = default;

	MaterialResource& material();
	const MaterialResource& material() const;

	std::unordered_map<std::string, ImageResource>& images();
	const std::unordered_map<std::string, ImageResource>& images() const;

private:
	MaterialResource assetMaterial;
	std::unordered_map<std::string, ImageResource> assetImages;
};

std::string serializeMaterialAsset(const MaterialAsset& asset, std::int32_t indent = 4);

MaterialAsset deserializeMaterialAsset(std::istream& stream);
MaterialAsset deserializeMaterialAsset(const std::string& data);
MaterialAsset deserializeMaterialAsset(const char* data, std::size_t size);

void to_json(nlohmann::ordered_json& j, const MaterialAsset& asset);
void from_json(const nlohmann::ordered_json& j, MaterialAsset& asset);
}