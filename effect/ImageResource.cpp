#include "ImageResource.h"
#include "../common/Compression.h"

namespace pixelpart {
ImageResource::ImageResource(const std::string& name, uint32_t w, uint32_t h, uint32_t bpp) : Resource(name),
	width(w), height(h), bitsPerPixel(bpp) {
	data.resize(width * height * bitsPerPixel / 8u);
}

void ImageResource::resize(uint32_t w, uint32_t h) {
	width = w;
	height = h;
	data.resize(width * height * bitsPerPixel / 8u);
}

uint32_t ImageResource::imageWidth() const {
	return width;
}
uint32_t ImageResource::imageHeight() const {
	return height;
}
uint32_t ImageResource::imageBitsPerPixel() const {
	return bitsPerPixel;
}

std::size_t ImageResource::imageDataSize() const {
	return data.size();
}

std::vector<unsigned char>& ImageResource::imageData() {
	return data;
}
const std::vector<unsigned char>& ImageResource::imageData() const {
	return data;
}

void to_json(nlohmann::ordered_json& j, const ImageResource& resource) {
	std::string compressedData = compressAndEncode(
		resource.imageData().data(), resource.imageData().size(), CompressionMethod::zlib);

	j = nlohmann::ordered_json{
		{ "name", resource.name() },
		{ "width", resource.imageWidth() },
		{ "height", resource.imageHeight() },
		{ "bpp", resource.imageBitsPerPixel() },
		{ "compression", CompressionMethod::zlib },
		{ "data", compressedData }
	};
}
void from_json(const nlohmann::ordered_json& j, ImageResource& resource) {
	resource = ImageResource(
		j.at("name"),
		j.value("width", 0u),
		j.value("height", 0u),
		j.value("bpp", 0u));

	resource.imageData() = decodeAndDecompress(
		j.at("data").get<std::string>(),
		resource.imageDataSize(),
		j.value("compression", CompressionMethod::none));
}
}