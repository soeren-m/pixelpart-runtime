#include "ImageResource.h"
#include "../common/Compression.h"
#include "../common/Json.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const ImageResource& resource) {
	std::string compressedData = compressAndEncode(resource.data.data(), resource.data.size(), CompressionMethod::zlib);

	j = nlohmann::ordered_json{
		{ "name", resource.name },
		{ "width", resource.width },
		{ "height", resource.height },
		{ "bpp", resource.bpp },
		{ "compression", CompressionMethod::zlib },
		{ "data", compressedData }
	};
}
void from_json(const nlohmann::ordered_json& j, ImageResource& resource) {
	fromJson(resource.name, j, "name"); // TODO

	resource.width = j.at("width");
	resource.height = j.at("height");
	resource.bpp = j.at("bpp");

	CompressionMethod compressionMethod = j.contains("compression") ? j.at("compression") : CompressionMethod::none;
	std::size_t uncompressedSize = resource.width * resource.height * resource.bpp / 8u;

	resource.data = decodeAndDecompress(j.at("data"), uncompressedSize, compressionMethod);
}
}