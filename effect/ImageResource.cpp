#include "ImageResource.h"
#include "../common/Compression.h"
#include <cstring>
#include <algorithm>

namespace pixelpart {
ImageResource::ImageResource(const std::string& name, uint32_t w, uint32_t h, uint32_t bpp) :
	Resource(name), width(w), height(h), bitsPerPixel(bpp) {
	data.resize(width * height * bitsPerPixel / 8u);
}
ImageResource::ImageResource(const std::string& name, uint32_t w, uint32_t h, uint32_t bpp, const unsigned char* source) :
	ImageResource(name, w, h, bpp) {
	copy(source, data.size());
}

void ImageResource::resize(uint32_t w, uint32_t h) {
	width = w;
	height = h;
	data.resize(width * height * bitsPerPixel / 8u, 0u);
}
void ImageResource::assign(uint32_t w, uint32_t h, unsigned char value) {
	width = w;
	height = h;
	data.assign(width * height * bitsPerPixel / 8u, value);
}
void ImageResource::clear(unsigned char value) {
	data.assign(width * height * bitsPerPixel / 8u, value);
}

void ImageResource::copy(const unsigned char* source, std::size_t size) {
	std::memcpy(&data.front(), source, std::min(size, data.size()));
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
		j.value("name", "unknown"),
		j.value("width", 0u),
		j.value("height", 0u),
		j.value("bpp", 0u));

	resource.imageData() = decodeAndDecompress(
		j.at("data").get<std::string>(),
		resource.imageDataSize(),
		j.value("compression", CompressionMethod::none));
}
}