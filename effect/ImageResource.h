#pragma once

#include "Resource.h"
#include "ColorSpace.h"
#include "../json/json.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace pixelpart {
class ImageResource : public Resource {
public:
	ImageResource() = default;
	ImageResource(const std::string& name, std::uint32_t w, std::uint32_t h, std::uint32_t ch, ColorSpace cs);
	ImageResource(const std::string& name, std::uint32_t w, std::uint32_t h, std::uint32_t ch, ColorSpace cs, const std::uint8_t* source);

	void resize(std::uint32_t w, std::uint32_t h);
	void assign(std::uint32_t w, std::uint32_t h, std::uint8_t value);
	void clear(std::uint8_t value);

	void copy(const std::uint8_t* source, std::size_t size);

	std::uint32_t width() const;
	std::uint32_t height() const;
	std::uint32_t channels() const;
	ColorSpace colorSpace() const;

	std::size_t dataSize() const;

	std::vector<std::uint8_t>& data();
	const std::vector<std::uint8_t>& data() const;

private:
	std::uint32_t imageWidth = 0;
	std::uint32_t imageHeight = 0;
	std::uint32_t imageChannels = 0;
	ColorSpace imageColorSpace = ColorSpace::linear;

	std::vector<std::uint8_t> imageData;
};

void to_json(nlohmann::ordered_json& j, const ImageResource& resource);
void from_json(const nlohmann::ordered_json& j, ImageResource& resource);
}