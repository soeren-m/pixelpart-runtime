#include "ImageResource.h"
#include "Base64.h"
#include "zlib/zlib.h"

namespace pixelpart {
void to_json(nlohmann::ordered_json& j, const ImageResource& resource) {
	if(resource.data.empty()) {
		throw std::runtime_error("Compression error");
	}

	uLongf compressedSize = (resource.data.size() * 1.1) + 12;
	std::vector<Bytef> compressedData(compressedSize);

	int result = compress(
		compressedData.data(),
		&compressedSize,
		&resource.data[0],
		resource.data.size());

	if(result != Z_OK) {
		throw std::runtime_error("zlib compression error");
	}

	j = nlohmann::ordered_json{
		{ "width", resource.width },
		{ "height", resource.height },
		{ "bpp", resource.bpp },
		{ "compression", "zlib" },
		{ "data", encodeBase64(static_cast<const unsigned char*>(compressedData.data()), static_cast<std::size_t>(compressedSize)) }
	};
}
void from_json(const nlohmann::ordered_json& j, ImageResource& resource) {
	resource.width = j.at("width");
	resource.height = j.at("height");
	resource.bpp = j.at("bpp");
	resource.data.clear();

	std::string compression = j.contains("compression") ? j.at("compression").get<std::string>() : "";
	std::string compressedData = decodeBase64(j.at("data").get<std::string>());

	if(compressedData.empty()) {
		throw std::runtime_error("Decompression error");
	}

	if(compression == "zlib") {
		uLongf uncompressedSize = resource.width * resource.height * resource.bpp / 8;
		resource.data.resize(uncompressedSize);

		int result = uncompress(
			&resource.data[0],
			&uncompressedSize,
			reinterpret_cast<const Bytef*>(compressedData.data()),
			static_cast<uLong>(compressedData.size()));

		if(result != Z_OK) {
			throw std::runtime_error("zlib decompression error");
		}
	}
	else if(compression.empty()) {
		resource.data = std::vector<unsigned char>(compressedData.begin(), compressedData.end());
	}
	else {
		throw std::runtime_error("Unknown compression method");
	}
}
}