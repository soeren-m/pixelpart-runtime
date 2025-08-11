#include "ImageResource.h"
#include "../common/Compression.h"
#include <cstring>
#include <algorithm>

namespace pixelpart {
ImageResource::ImageResource(const std::string& name, std::uint32_t w, std::uint32_t h, std::uint32_t ch, ColorSpace cs) :
	Resource(name), imageWidth(w), imageHeight(h), imageChannels(ch), imageColorSpace(cs) {
	imageData.resize(imageWidth * imageHeight * imageChannels);
}
ImageResource::ImageResource(const std::string& name, std::uint32_t w, std::uint32_t h, std::uint32_t ch, ColorSpace cs, const std::uint8_t* source) :
	ImageResource(name, w, h, ch, cs) {
	copy(source, imageData.size());
}

void ImageResource::resize(std::uint32_t w, std::uint32_t h) {
	imageWidth = w;
	imageHeight = h;
	imageData.resize(imageWidth * imageHeight * imageChannels, 0);
}
void ImageResource::assign(std::uint32_t w, std::uint32_t h, std::uint8_t value) {
	imageWidth = w;
	imageHeight = h;
	imageData.assign(imageWidth * imageHeight * imageChannels, value);
}
void ImageResource::clear(std::uint8_t value) {
	imageData.assign(imageWidth * imageHeight * imageChannels, value);
}

void ImageResource::copy(const std::uint8_t* source, std::size_t size) {
	std::memcpy(&imageData.front(), source, std::min(size, imageData.size()));
}

std::uint32_t ImageResource::width() const {
	return imageWidth;
}
std::uint32_t ImageResource::height() const {
	return imageHeight;
}
std::uint32_t ImageResource::channels() const {
	return imageChannels;
}
ColorSpace ImageResource::colorSpace() const {
	return imageColorSpace;
}

std::size_t ImageResource::dataSize() const {
	return imageData.size();
}

std::vector<std::uint8_t>& ImageResource::data() {
	return imageData;
}
const std::vector<std::uint8_t>& ImageResource::data() const {
	return imageData;
}

void to_json(nlohmann::ordered_json& j, const ImageResource& resource) {
	std::string compressedData = compressBase64(
		resource.data().data(), resource.data().size(), CompressionMethod::zlib);

	j = nlohmann::ordered_json{
		{ "name", resource.name() },
		{ "width", resource.width() },
		{ "height", resource.height() },
		{ "channels", resource.channels() },
		{ "color_space", resource.colorSpace() },
		{ "compression", CompressionMethod::zlib },
		{ "data", compressedData }
	};
}
void from_json(const nlohmann::ordered_json& j, ImageResource& resource) {
	std::uint32_t channels = j.value("channels", 0u);
	if(channels == 0) {
		channels = j.value("bpp", 0u) / 8;
	}

	resource = ImageResource(
		j.value("name", "unknown"),
		j.value("width", 0u),
		j.value("height", 0u),
		channels,
		j.value("color_space", ColorSpace::srgb));

	resource.data() = decompressBase64(
		j.at("data").get<std::string>(),
		resource.dataSize(),
		j.value("compression", CompressionMethod::none));
}
}
