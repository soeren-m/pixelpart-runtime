#pragma once

#include "Resource.h"
#include "../json/json.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace pixelpart {
class ImageResource : public Resource {
public:
	ImageResource() = default;
	ImageResource(const std::string& name, uint32_t w, uint32_t h, uint32_t bpp);
	ImageResource(const std::string& name, uint32_t w, uint32_t h, uint32_t bpp, const unsigned char* source);

	void resize(uint32_t w, uint32_t h);
	void assign(uint32_t w, uint32_t h, unsigned char value);
	void clear(unsigned char value);

	void copy(const unsigned char* source, std::size_t size);

	uint32_t imageWidth() const;
	uint32_t imageHeight() const;
	uint32_t imageBitsPerPixel() const;

	std::size_t imageDataSize() const;

	std::vector<unsigned char>& imageData();
	const std::vector<unsigned char>& imageData() const;

private:
	uint32_t width = 0u;
	uint32_t height = 0u;
	uint32_t bitsPerPixel = 0u;

	std::vector<unsigned char> data;
};

void to_json(nlohmann::ordered_json& j, const ImageResource& resource);
void from_json(const nlohmann::ordered_json& j, ImageResource& resource);
}