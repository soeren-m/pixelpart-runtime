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
	ImageResource(const std::string& name, uint32_t w, uint32_t h, uint32_t ch, ColorSpace cs);
	ImageResource(const std::string& name, uint32_t w, uint32_t h, uint32_t ch, ColorSpace cs, const uint8_t* source);

	void resize(uint32_t w, uint32_t h);
	void assign(uint32_t w, uint32_t h, uint8_t value);
	void clear(uint8_t value);

	void copy(const uint8_t* source, std::size_t size);

	uint32_t width() const;
	uint32_t height() const;
	uint32_t channels() const;
	ColorSpace colorSpace() const;

	std::size_t dataSize() const;

	std::vector<uint8_t>& data();
	const std::vector<uint8_t>& data() const;

private:
	uint32_t imageWidth = 0u;
	uint32_t imageHeight = 0u;
	uint32_t imageChannels = 0u;
	ColorSpace imageColorSpace = ColorSpace::linear;

	std::vector<uint8_t> imageData;
};

void to_json(nlohmann::ordered_json& j, const ImageResource& resource);
void from_json(const nlohmann::ordered_json& j, ImageResource& resource);
}