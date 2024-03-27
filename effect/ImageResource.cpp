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
	resource = ImageResource();

	fromJson(resource.name, j, "name");
	fromJson(resource.width, j, "width");
	fromJson(resource.height, j, "height");
	fromJson(resource.bpp, j, "bpp");

	CompressionMethod compressionMethod = CompressionMethod::none;
	fromJson(compressionMethod, j, "compression");

	std::size_t uncompressedSize = resource.width * resource.height * resource.bpp / 8u;
	resource.data = decodeAndDecompress(j.at("data").get<std::string>(), uncompressedSize, compressionMethod);
}
}